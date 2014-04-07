//
#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <Shaders/TerrainShadersCommon.inc>
#include <Shaders/DeferredShadingHelpers.inc>

#ifdef DYNLIGHT_SPOT_PROJECTOR
  #define DYNLIGHT_USE_REFPLANES
#endif

#pragma warning(default:7203)
#if defined(DYNLIGHT) && !defined(DYNLIGHT_DIRECTIONAL)
  #define DYNLIGHT_POS
#endif



#ifdef _VISION_DX11
cbuffer g_GlobalConstantBufferFrame : register (b0)
{
  #ifdef DEPTHFILL
  float4   ClipPlanes  : packoffset(c12);  // context clip planes: near, far, 1/(far-near)
  #endif
  float4   timer       : packoffset(c16);  // Time: timer, time difference
  float4   eyePos      : packoffset(c17);  // eye position
  float4   depthFog    : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
  float4   depthFogCol : packoffset(c20);  // linear depth fog color
  float4   gAmbientCol     : packoffset(c21);  // global ambient color (deferred)
  float4   _NoExport_lightmapMul : packoffset(c22);  // lightmap multiplier (skalar)
  float4   SMLightColor : packoffset(c23);  
  #ifdef USE_SUNLIGHT
    float3    vSunDirection   : packoffset(c24);
    float3    vSunColor       : packoffset(c25);
  #endif
}
cbuffer g_GlobalConstantBufferObject : register (b1)
{
  float4x4 matMV       : packoffset(c0);   // model view matrix
  float4x4 matMVP      : packoffset(c4);   // model view projection matrix
  float4x4 matMtoW     : packoffset(c8);   // model to worldspace matrix
  float4   LMtoSM      : packoffset(c12);  // lightmap to shadowmap: xy:scale, zw:ofs
  float4   MUVtoLM     : packoffset(c13);  // model UV to lightmap
}


#ifdef _IS_VERTEX_SHADER
#ifndef SKIP_DEFAULT_CONSTANT_BUFFER
cbuffer g_GlobalConstantBufferUserVS : register (b2)
{
  #ifdef DYNLIGHT_POS
    float4 LightPos       : packoffset(c0);
  #endif
 
  #ifdef DEPTHFILL
    float4 DepthBias  : packoffset(c1);
  #endif
  
  #ifdef DEPTHFILL_SHADOWS
    float4 DepthBias  : packoffset(c1);
    float4 ClipPlanes : packoffset(c2);
  #endif
  
  #ifdef DYNLIGHT_DIRECTIONAL
    float4 LightDir : packoffset(c1);
  #endif
  
  #ifdef DYNLIGHT_USE_REFPLANES
    float4 RefPlaneX      : packoffset(c2);
    float4 RefPlaneY      : packoffset(c3);
    float4 RefPlaneW      : packoffset(c4);
  #endif
}
#endif // SKIP_DEFAULT_CONSTANT_BUFFER

#endif // _IS_VERTEX_SHADER

#ifdef _IS_PIXEL_SHADER

#ifndef SKIP_DEFAULT_CONSTANT_BUFFER
cbuffer g_GlobalConstantBufferUserPS : register (b2)
{
  float4 LightRadius;    // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
  float4 LightColor;     // xyz: color with pre-multiplied light multiplier
  
  #ifdef DYNLIGHT_DIRECTIONAL
    float3 LightDirPS;
  #endif
  
  #ifdef SHADOWMAP
    float2 InvScreenSize;
  #endif
}

#endif //BLOB_SHADOWS

#ifdef VMATERIAL_USEDIRECTIONAL
cbuffer g_GlobalConstantBufferDirLight : register (b5)
{
  float4 _NoExport_LightDirection;
  float4 _NoExport_DirectionalLightColor;
  float4 _NoExport_AmbientColor;
}
#endif



#endif //_IS_PIXEL_SHADER

  #ifdef DYNLIGHT_POS
    Texture2D AttenTex                : register(t7);      //Attenuation curve
    sampler   AttenTexSampler         : register(s7);      //Attenuation curve
  #endif

  #ifdef DYNLIGHT_OMNI_PROJECTOR
    TextureCube ProjTex           : register(t13);
    sampler     ProjTexSampler    : register(s13);
  #endif
  #ifdef DYNLIGHT_SPOT_PROJECTOR
    Texture2D   ProjTex           : register(t13);
    sampler     ProjTexSampler    : register(s13);
  #endif
  #ifdef SHADOWMAP
    Texture2D   ShadowTex         : register(t14);
    sampler     ShadowTexSampler  : register(s14);
  #endif

#else // _VISION_DX11

#ifdef _IS_VERTEX_SHADER
  // vertex shader constants
  float4x4 matMV            : register(c0);
  float4x4 matMVP           : register(c8);
  float4x4 matMtoW          : register(c20);
  float4   contextCP        : register(c24);
  float4   LMtoSM           : register(c28);
  float4   SMLightColor     : register(c29);
  float4   timer            : register(c30);
#endif // _IS_VERTEX_SHADER


  #ifdef DYNLIGHT_DIRECTIONAL
    float3 LightDir         : register(c35);
  #endif  

  #if defined(USE_SUNLIGHT) && defined(_IS_VERTEX_SHADER)
    float3 vSunDirection   : register(c56);
    float3 vSunColor       : register(c57);
  #endif
  
  float4   MUVtoLM          : register(c60); // transforms global lightmap coords to page
  float4   depthFog         : register(c62); // vs

  #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
    #ifdef DYNLIGHT_POS
      float4 LightPos : register(c64);
    #endif
    #ifdef DYNLIGHT_USE_REFPLANES
      float4 RefPlaneX : register(c66);
      float4 RefPlaneY : register(c67);
      float4 RefPlaneW : register(c68);
    #endif
    #ifdef DEPTHFILL
      float4 ClipPlanes : register(c24);
      float4 DepthBias  : register(c70);
    #endif
    #ifdef DEPTHFILL_SHADOWS
      float4 ClipPlanes : register(c64);
      float4 DepthBias  : register(c65);
    #endif    
  #else
    #ifdef DYNLIGHT_POS
      float4 LightPos;
    #endif
    #ifdef DYNLIGHT_USE_REFPLANES
      float4 RefPlaneX;
      float4 RefPlaneY;
      float4 RefPlaneW;
    #endif
    #ifdef DEPTHFILL
      float4 ClipPlanes : register(c24);
      float4 DepthBias;
    #ifdef DEPTHFILL_SHADOWS
      float4 ClipPlanes : register(c64);
      float4 DepthBias  : register(c65);
    #endif  
  #endif
  #endif

  // pixel shader constants
  float4   _NoExport_lightmapMul      : register(c0);
  float4   depthFogCol      : register(c1);  // ps
  float3   gAmbientCol      : register(c21); // deferred
  
  #ifdef DYNLIGHT
    float4 LightColor  :register(c33); // xyz: color with pre-multiplied light multiplier
    float4 LightRadius :register(c34); // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
  #endif
  #ifdef DYNLIGHT_DIRECTIONAL
    float3 LightDirPS       : register(c35);
  #endif    
  #ifdef SHADOWMAP
    float2 InvScreenSize : register(c36);
  #endif 

#ifdef _IS_PIXEL_SHADER
  #ifdef VMATERIAL_USEDIRECTIONAL
    float4 _NoExport_LightDirection : register(c26);
    float4 _NoExport_DirectionalLightColor : register(c27);
    float4 _NoExport_AmbientColor   : register(c28);
  #endif
#endif

  #ifdef DYNLIGHT_POS
    sampler2D AttenTex          : register(s7);      //Attenuation curve
  #endif

  #ifdef DYNLIGHT_OMNI_PROJECTOR
    samplerCUBE ProjTex           : register(s13);
  #endif
  #ifdef DYNLIGHT_SPOT_PROJECTOR
    sampler2D   ProjTex           : register(s13);
  #endif
  #ifdef SHADOWMAP
    sampler2D   ShadowTex         : register(s14);
#endif
#endif


#if defined(VMATERIAL_DOT3LIGHTMAPS) && !defined(VMATERIAL_USELIGHTMAPS)
  #define VMATERIAL_USELIGHTMAPS
#endif

struct VS_OUT
{
  float4 ProjPos  : SV_Position;
#if defined(FOR_MIRROR) && defined(USE_SUNLIGHT)
  float3 Color : COLOR0;
#endif
#ifdef DEPTHFILL
  float Depth : TEXCOORD0;
#else
    float4 baseUV : TEXCOORD0;  // xy=global UV, zw=sector UV
  #ifdef VMATERIAL_USELIGHTMAPS
    float2 LightmapUV : TEXCOORD1;
  #endif
  #ifdef USE_FOG
    float fogCoord : TEXCOORD2;
  #endif
  #ifdef USE_SHADOWMAP
    float4 LightColor : TEXCOORD2;
  #endif
  #ifdef DYNLIGHT_POS
    float3 LightWS : TEXCOORD3;
  #endif
  #ifdef DYNLIGHT_OMNI_PROJECTOR
    float3 ProjDir : TEXCOORD4;
  #endif
  #ifdef DYNLIGHT_SPOT_PROJECTOR
    float4 ProjTexCoord : TEXCOORD4;
  #endif
  #ifdef USE_DEFERRED
    float Depth : TEXCOORD3;
  #endif
  #if defined(FOR_MIRROR) && defined(USE_SUNLIGHT)
    float3 LightDirection : TEXCOORD4;
  #endif
  #ifdef SHADOWMAP
    float4 ScreenPos : TEXCOORD5;
  #endif
  float4 WorldUV : TEXCOORD6;
  
  #if defined(SINGLE_DETAIL_PASS)
	float4 SinglePassDetailPassTiling : TEXCOORD7;
  #endif

#endif

};



#ifdef _IS_VERTEX_SHADER

VS_OUT vs_main( VS_IN In )
{
   VS_OUT Out;
   float4 wsPos = GetVertexPosition(In);
   Out.ProjPos = mul( matMVP, wsPos );
   
  #ifdef DEPTHFILL
    float fDepth = Out.ProjPos.w;
    //Out.Depth = (fDepth /*- contextCP.x*/) / contextCP.y;
    Out.Depth = fDepth / ClipPlanes.y;
  #else
    
   Out.WorldUV = GetWorldUV(In);
   Out.baseUV = GetUV(In.ObjPosXY);
  #ifdef USE_FOG	
    #ifdef USE_MIRROR_CLIP_PLANE
        Out.fogCoord = GetFogCoord(depthFog, dot(matMV._m20_m21_m22_m23, wsPos));
    #else
      Out.fogCoord = GetFogCoord(depthFog, Out.ProjPos.z);
    #endif
  #endif
  #ifdef VMATERIAL_USELIGHTMAPS
     // Per sector UV to lightmap - note that the baseUV are clamped to avoid filtering across page boundaries,
     // but the lightmap transform has been adjusted to undo the clamping, so the lightmap UVs will be correct
     Out.LightmapUV = Out.baseUV.zw * MUVtoLM.xy + MUVtoLM.zw; 
  #endif
  
  #if defined(FOR_MIRROR) && defined(USE_SUNLIGHT)
    Out.Color = vSunColor;
    Out.LightDirection = vSunDirection;
  #endif
  
  #ifdef DYNLIGHT_POS
     Out.LightWS = LightPos.xyz - wsPos;
  #endif
    #ifdef DYNLIGHT_OMNI_PROJECTOR
      // rather use existing matrix (same for spotlights):
      Out.ProjDir.x = dot(Out.LightWS,RefPlaneX.xyz);
      Out.ProjDir.y = dot(Out.LightWS,RefPlaneY.xyz);
      Out.ProjDir.z = dot(Out.LightWS,RefPlaneW.xyz);
    #endif
    #ifdef DYNLIGHT_SPOT_PROJECTOR
      //Compute projected texture coordinate
      Out.ProjTexCoord.x = dot(RefPlaneX, wsPos);
      Out.ProjTexCoord.y = dot(RefPlaneY, wsPos);
      Out.ProjTexCoord.z = 0.0f;
      Out.ProjTexCoord.w = dot(RefPlaneW, wsPos);
      float4 bias = Out.ProjTexCoord.w*float4(0.5f, 0.5f, 0.0f, 0.0f);
      Out.ProjTexCoord += bias;
    #endif


#endif // DEPTHFILL

   #ifdef SHADOWMAP
     Out.ScreenPos = Out.ProjPos;
     Out.ScreenPos.y = -Out.ScreenPos.y;
   #endif

   return Out;
}


#endif


#ifdef _IS_PIXEL_SHADER

float3x3 ComputeTerrainTBN(float3 Normal)
{
  float3 Tangent = float3 (1.0, 0.0, 0.0);
  float3 BiNormal = cross(Tangent, Normal);
  Tangent = cross(Normal, BiNormal);
  
  return float3x3(Tangent, -BiNormal, Normal);
}

float4 ps_main( VS_OUT In ) : SV_Target
{
#ifdef DEPTHFILL

  #ifdef _VISION_PS3
    return COMPUTE_CONVERTED_DEPTH(In.Depth);
  #else
    return float4(In.Depth, In.Depth, In.Depth, 1.0f);
  #endif
  
#else
  float2 normalUV = In.baseUV.zw * _NoExport_NormalmapTransform.xy + _NoExport_NormalmapTransform.zw;
  float3 worldNormal = normalize(vTex2D(Normalmap, NormalmapSampler, normalUV).xyz*2.0f-1.0f);

  #ifdef USE_3WAY
    TerrainLayerConfig config = CreateTerrainLayerConfig3Way(In.baseUV.zw, In.WorldUV.xyz, worldNormal);
  #else
    TerrainLayerConfig config = CreateTerrainLayerConfig(In.baseUV.zw, In.WorldUV.xyz, worldNormal);
  #endif

  float4 diffuse = GetTerrainDiffuseColor(config);
  
  #ifdef VMATERIAL_NORMALMAP
    float4 mixedNormals = GetTerrainDetailNormal(config);
    float4 normalmap = DecodeNormalAndAlpha(mixedNormals);
    normalmap.xyz = normalize(normalmap.xyz);
  #endif


#ifdef DYNLIGHT
  float LightInt = 1;
  
  #ifndef DYNLIGHT_DIRECTIONAL
    float LightIntR = length(In.LightWS.xyz) * LightRadius.y;  // dist 0..1 for 0..r. Let the sampler clamp it to [0..1], not the shader
    LightInt = vTex2D(AttenTex, AttenTexSampler, float2(LightIntR, 0.0f)).x; //custom attenuation curve texture
  #endif

  #ifdef DYNLIGHT_DIRECTIONAL
    float3 NormLightVec = -LightDirPS;
  #else
    float3 NormLightVec = normalize(In.LightWS);
  #endif
  
  float DotProd = saturate(dot(NormLightVec.xyz, worldNormal.xyz));

  #ifdef DYNLIGHT_OMNI_PROJECTOR
    #ifdef DYNLIGHT_OMNI_PROJECTOR_DISTANCEMIPMAP
      // use a lower mipmap according to distance (assuming hardcoded first 4 miplevels)
      float4 ProjLightCol = LightColor*vTexCUBEbias(ProjTex, ProjTexSampler, float4(In.ProjDir,LightIntR*4.0));
    #else
      float4 ProjLightCol = LightColor*vTexCUBE(ProjTex, ProjTexSampler, In.ProjDir); // Cubemap projector
    #endif
  #elif defined(DYNLIGHT_SPOT_PROJECTOR)
    float4 ProjLightCol = LightColor*vTex2Dproj(ProjTex, ProjTexSampler, In.ProjTexCoord); //2D spotlight projector
  #else
    float4 ProjLightCol = LightColor; //No projector
  #endif

  float4 result = diffuse * LightInt * ProjLightCol * DotProd;
  #ifdef SHADOWMAP
    float2 ScreenTex = GetScreenTexPos(In.ScreenPos, InvScreenSize);

    #ifdef _VISION_XENON
      // Note: shadow information is only stored in the
      //   green channel, thus we avoid to overwrite the
      //   stencil values in EDRAM on xbox360 in case of
      //   interleaved shadow map rendering
	  result.xyz *= vTex2D(ShadowTex, ShadowTexSampler, ScreenTex).g;
    #else
      result.xyz *= vTex2D(ShadowTex, ShadowTexSampler, ScreenTex).rgb;
    #endif
  #endif
  
#elif defined(FOR_MIRROR)
  #ifdef VMATERIAL_USEDIRECTIONAL
    float fDot = saturate(dot(worldNormal.xyz,_NoExport_LightDirection.xyz));
  
    float4 result = _NoExport_AmbientColor + fDot*_NoExport_DirectionalLightColor;
    result.xyz *= diffuse;
  #else
    #ifdef FULLBRIGHT
      float4 result = diffuse;
    #else
      #ifdef VMATERIAL_USELIGHTMAPS
        float4 lightColor = vTex2D(Lightmap, LightmapSampler, In.LightmapUV.xy) * _NoExport_lightmapMul.x; 
      #else
        float4 lightColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
      #endif
    
      #ifdef USE_SUNLIGHT
        float fDotProd = saturate(-dot(worldNormal.xyz, In.LightDirection));
      
        lightColor.xyz += fDotProd * In.Color;
      #endif
      #ifdef USE_AMBIENT_LIGHT
        lightColor.xyz += gAmbientCol.xyz;
      #endif
      float4 result = diffuse * lightColor;
    #endif
  #endif // VMATERIAL_USEDIRECTIONAL
  
#else
  #ifdef VMATERIAL_DOT3LIGHTMAPS
    float4 lightmap1 = vTex2D(Lightmap1, Lightmap1Sampler,In.LightmapUV.xy); 
    float4 lightmap2 = vTex2D(Lightmap2, Lightmap2Sampler,In.LightmapUV.xy); 
    float4 lightmap3 = vTex2D(Lightmap3, Lightmap3Sampler,In.LightmapUV.xy);
    float4 lightmap;
    lightmap.xyz = GetLightmapDot3LightingDiffuse(normalmap.xyz,lightmap1,lightmap2,lightmap3) * _NoExport_lightmapMul.x;
    lightmap.w = 1.0f;
  #else
    #ifdef VMATERIAL_USELIGHTMAPS
      float4 lightmap = vTex2D(Lightmap, LightmapSampler, In.LightmapUV.xy) * _NoExport_lightmapMul.x; 
    #endif
  #endif

  #ifdef VMATERIAL_USELIGHTMAPS
    lightmap.xyz += gAmbientCol.xyz;
    float4 result = diffuse * lightmap;
  #else
    #ifdef VMATERIAL_USEDIRECTIONAL
      #ifdef VMATERIAL_NORMALMAP
        float3x3 TBN = ComputeTerrainTBN (worldNormal.xyz);
        float3 normal = mul (normalmap.xyz, TBN);
        float fDot = saturate(dot(normal.xyz,_NoExport_LightDirection.xyz));
      #else
        float fDot = saturate(dot(worldNormal.xyz,_NoExport_LightDirection.xyz));
      #endif
    
    float4 result = _NoExport_AmbientColor + fDot*_NoExport_DirectionalLightColor;
    result.xyz *= diffuse.xyz;
    #else
      float4 result = diffuse; // fullbright
    #endif
  #endif
#endif

#ifdef USE_FOG
  result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, In.fogCoord);
#endif


  return result;
  
#endif // DEPTHFILL
}

#endif
