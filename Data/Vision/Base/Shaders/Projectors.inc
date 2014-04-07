#include <Shaders/VisionCommon.inc>

#ifdef VDEFERRED_NORMALMAPPED
  #include <Shaders/DeferredShadingHelpers.inc>
  #include <Shaders/ShaderHelpers.inc>
  #ifdef VTERRAIN
    //#include <Shaders/TerrainShaders.inc>
  #endif
#endif

struct VS_OUT_PROJ
{
  float4 ProjPos  : SV_Position;
  float4 UVProj : TEXCOORD0;
  float Attenuation : TEXCOORD1;
  float4 Color : COLOR;
  #ifdef USE_LIGHTMAP
    float2 UVLightmap : TEXCOORD2;
  #endif
#ifdef VDEFERRED_NORMALMAPPED
  #ifdef VTERRAIN
    float2 baseUV : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
  #else
    float3 Normal : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
    float3 BiTangent: TEXCOORD5;
  #endif
#endif
};


#ifdef _IS_VERTEX_SHADER
  #include <Shaders/SkinningVS.inc>

  #ifdef _VISION_DX11
    cbuffer g_GlobalConstantBufferObject : register (b1)
    {
      float4x4 matMVP  : packoffset(c4);   // model view projection matrix
      float4x4 matM2WS : packoffset(c8);   // model to worldspace matrix
      float4   MUVtoLM : packoffset(c13);  // model UV to lightmap
    }

    #ifdef VTERRAIN
      cbuffer g_GlobalConstantBufferTerrain : register (b3)
      {
        float4 SectorScaleOfs   : packoffset(c0); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
        float4 UVScaleOfs       : packoffset(c1); // maps incoming [0..1] sector UV range to global [0..1] range; xy: scaling, zw:ofs
        float4 UVSectorScaleOfs : packoffset(c2); // maps incoming [0..1] mesh UV range to sector [0..1] range; xy: scaling, zw:ofs
        float4 _NoExport_NormalmapTransform : packoffset(c5); // register to store scale/bias value to map the normalmap UV (remapping for the overlapping at sector borders)
      }
    #endif // VTERRAIN
  #else // _VISION_DX11
    float4x4 matMVP  : register(c8);
    float4x4 matM2WS : register(c20);
    float4 MUVtoLM   : register(c60);

    #ifdef VTERRAIN
      float4 SectorScaleOfs   : register(c38); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
      float4 UVScaleOfs       : register(c39); // maps incoming [0..1] sector UV range to global [0..1] range; xy: scaling, zw:ofs
      float4 UVSectorScaleOfs : register(c40); // maps incoming [0..1] mesh UV range to sector [0..1] range; xy: scaling, zw:ofs
    #endif // VTERRAIN

  #endif // _VISION_DX11
    
  #if defined(_VISION_DX11)
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
      float4 PrimaryColor : packoffset(c0);
      float4 RefPlaneS    : packoffset(c1);
      float4 RefPlaneT    : packoffset(c2);
      float4 RefPlaneQ    : packoffset(c3);
      float4 RefPlaneAtt  : packoffset(c4);
    }
  #else
    float4 PrimaryColor : register(c64);
    float4 RefPlaneS    : register(c65);
    float4 RefPlaneT    : register(c66);
    float4 RefPlaneQ    : register(c67);
    float4 RefPlaneAtt  : register(c68);
  #endif

  //////////////////////////////////////////////////////////////////////////
  // vertex shader	
  //////////////////////////////////////////////////////////////////////////

  struct VS_IN      
  { 
    #ifdef VTERRAIN
      float2 ObjPosXY : POSITION; // shared stream
      float ObjPosZ : TEXCOORD0;  // sector's own stream
    #else
      float3 ObjPos   : POSITION;  
      #ifdef VDEFERRED_NORMALMAPPED
        #ifndef VTERRAIN
        float3 Normal   : NORMAL;
        #endif
      #endif
      #ifdef USE_LIGHTMAP
        float2 UVLightmap : TEXCOORD1;
      #endif
 
      SKINNING_VERTEX_STREAMS
    #endif
  };


  // calculates the object space vertex position
  float4 GetVertexPosition(VS_IN In)
  {
    #ifdef VTERRAIN
      float4 pos = float4(In.ObjPosXY.x,In.ObjPosXY.y,In.ObjPosZ,1.0);
      pos.xy = pos.xy*SectorScaleOfs.xy + SectorScaleOfs.zw;
      return pos;
    #else
      return float4(In.ObjPos,1.0);
    #endif 
  }

  // object space to world space
  float4 ToWorldSpace(float4 pos)
  {
    #ifdef VTERRAIN
      return pos;
    #else
      pos = mul( matM2WS, pos);
      return float4(pos.xyz,1.0);
    #endif 
  }


  #ifndef USE_SKINNING
    VS_OUT_PROJ vs_main( VS_IN In )
  #else
    VS_OUT_PROJ vs_main_skinning( VS_IN In )
  #endif
  {
    VS_OUT_PROJ Out;
    #ifndef USE_SKINNING
      float4 ObjPos4 = GetVertexPosition(In);
    #else
      float4 ObjPos4;
      PREPARE_SKINNING(In.BoneIndices);
      TRANSFORM_OBJECT_POS(ObjPos4, In.ObjPos, In.BoneWeight);
    #endif
   
    Out.ProjPos = mul( matMVP, ObjPos4);
    Out.Color = PrimaryColor;
    float4 wsPos = ToWorldSpace(ObjPos4);
    float distq = dot(wsPos, RefPlaneQ);
    float dists = dot(wsPos, RefPlaneS);
    float distt = dot(wsPos, RefPlaneT);
    Out.UVProj.x = 0.5 * distq + dists;
    Out.UVProj.y = 0.5 * distq + distt;
    Out.UVProj.z = 0.f;
    Out.UVProj.w = distq;
    Out.Attenuation = dot(wsPos, RefPlaneAtt);

    #ifdef USE_LIGHTMAP
      #ifdef VTERRAIN
        float2 lightmapUV = In.ObjPosXY.xy*UVSectorScaleOfs.xy+UVSectorScaleOfs.zw;
        Out.UVLightmap = lightmapUV * MUVtoLM.xy + MUVtoLM.zw; // per sector UV to lightmap
      #else
        Out.UVLightmap = In.UVLightmap * MUVtoLM.xy + MUVtoLM.zw; // lightmap transform
      #endif
    #endif

    #ifdef VDEFERRED_NORMALMAPPED
    #ifdef VTERRAIN
      // can only prepare tangent space here because we have the normal only in the PS
      Out.Tangent = RefPlaneS.xyz;
      Out.baseUV = In.ObjPosXY.xy*UVSectorScaleOfs.xy+UVSectorScaleOfs.zw;
    #else
      // build tangent space
      Out.Normal = In.Normal;
      Out.Tangent = RefPlaneS.xyz;
      Out.BiTangent = normalize(cross(Out.Normal, Out.Tangent));
      Out.Tangent = normalize(cross(Out.BiTangent, Out.Normal));
    #endif // VTERRAIN
    #endif // VDEFERRED_NORMALMAPPED
    return Out;
  }
#else

  //////////////////////////////////////////////////////////////////////////
  // pixel shader	
  //////////////////////////////////////////////////////////////////////////

  #ifdef _VISION_DX11
    Texture2D WmarkTex        : register(t0);
    sampler   WmarkTexSampler : register(s0);
    Texture2D Lightmap        : register(t1);
    sampler   LightmapSampler : register(s1);
    Texture2D NormalMap        : register(t2);
    sampler   NormalMapSampler : register(s2);
    Texture2D SectorNormalmap        : register(t3);
    sampler   SectorNormalmapSampler : register(s3);

    cbuffer g_GlobalConstantBufferFrame : register (b0)
    {
      float4    gAmbientCol     : packoffset(c21);  // global ambient color
      float4    LightmapMultiplier : packoffset(c22);  // lightmap multiplier (skalar) 
    }

    #ifdef VTERRAIN
      cbuffer g_GlobalConstantBufferTerrain : register (b3)
      {
        float4 SectorScaleOfs   : packoffset(c0); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
        float4 UVScaleOfs       : packoffset(c1); // maps incoming [0..1] sector UV range to global [0..1] range; xy: scaling, zw:ofs
        float4 UVSectorScaleOfs : packoffset(c2); // maps incoming [0..1] mesh UV range to sector [0..1] range; xy: scaling, zw:ofs
        float4 _NoExport_NormalmapTransform : packoffset(c5); // register to store scale/bias value to map the normalmap UV (remapping for the overlapping at sector borders)
      }
    #endif // VTERRAIN

    
  #else
    sampler2D WmarkTex            : register(s0);
    sampler2D Lightmap            : register(s1);
    sampler2D NormalMap           : register(s2);
    sampler2D SectorNormalmap     : register(s3);
    float4    LightmapMultiplier  : register(c0);
    float4 _NoExport_NormalmapTransform : register(c7); // register to store scale/bias value to map the normalmap UV (remapping for the overlapping at sector borders)
    float3 gAmbientCol            : register(c21);
  #endif


#define PS_IN_PROJ VS_OUT_PROJ
  
#ifdef VDEFERRED_NORMALMAPPED

PS_OUT ps_main( PS_IN_PROJ In )
{
  float4 diffuse = vTex2Dproj(WmarkTex, WmarkTexSampler, In.UVProj)*In.Color;

  float fAttSmooth = smoothstep(0,1, abs(In.Attenuation));
  diffuse.a *= (1.0f-fAttSmooth);
  
  clip(diffuse.a-0.01); // perform alpha test to leave the other texels un-touched
      
  
  // sample normal map and transform to world space  
  float4 normalMap = vTex2Dproj(NormalMap, NormalMapSampler, In.UVProj) * 2.0 - 1.0;
  
  #ifdef VTERRAIN
    float2 normalUV = In.baseUV.xy * _NoExport_NormalmapTransform.xy + _NoExport_NormalmapTransform.zw;
    float4 normal = vTex2D(SectorNormalmap, SectorNormalmapSampler, normalUV); 
    normal.xyz = normalize(normal.xyz*2-1);

    float3 tangent = In.Tangent; //  == RefPlaneS;
    float3 biTangent = normalize(cross(normal, tangent));
    tangent = normalize(cross(biTangent, normal));
    float3 worldNormal = GetWorldNormal(normalMap, tangent, biTangent, normal);

  #else
    float3 worldNormal = GetWorldNormal(normalMap, In.Tangent, In.BiTangent, In.Normal);
  #endif
  
  PixelDataUnpacked pixelData;
  
  pixelData.vAccInitial = diffuse.rgb * gAmbientCol.rgb;
  pixelData.vNormal = worldNormal;
  pixelData.vDiffColor = diffuse.rgb;
  pixelData.fSpecExp = 1.f; 
  pixelData.fMaterial = 0.f;

  // we dont write into that buffer
  pixelData.fDepth = 0.0f; 
  pixelData.vSpecColor = float3(1,0,0);
 
  PS_OUT result = packPixelValues(pixelData);
  
  result.Accumulation.a = diffuse.a; // alpha value
  result.Diffuse.a = diffuse.a; // alpha value
  result.Normal.a = diffuse.a; // alpha value
  return result;
}

#else

  float4 ps_main( PS_IN_PROJ In ) : SV_Target
  {
    float4 result = vTex2Dproj(WmarkTex, WmarkTexSampler, In.UVProj)*In.Color;

    // smoothstep is only supported for SM2.0
    float fAttSmooth = smoothstep(0,1, abs(In.Attenuation));

    #ifdef NO_BORDERCOLOR_SUPPORT
    
      // Note: Not intializing b4Out at construction time makes the compilation fail
      // on the Google Nexus.
      bool4 b4Out = float4(In.UVProj.xy, 0.0f, 0.0f) > In.UVProj.wwxy;
      float fMul = float(!any(b4Out));
    
      result *= fMul;
      
    #endif
    
    #ifdef USE_LIGHTMAP
      float4 lightmap = vTex2D(Lightmap, LightmapSampler, In.UVLightmap);
      
      lightmap *= LightmapMultiplier.x;
      
      result.xyz *= lightmap; 
    #endif

    result *= (1-fAttSmooth); // this works for multiplicative blending
    // result.w *= (1-fAttSmooth); // this would work better for alpha blending
    return result;
  }
#endif // VDEFERRED_NORMALMAPPED


#endif
