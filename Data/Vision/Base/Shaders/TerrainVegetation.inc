//
#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

#ifdef USE_DEFERRED
  #include <Shaders/DeferredShadingHelpers.inc>
#endif 

#ifdef USE_LIGHTGRID
  #define USE_LIGHTGRID_VS
#endif

#ifdef VMATERIAL_NORMALMAP
  #undef USE_LIGHTGRID_VS
#endif

#ifdef USE_LIGHTGRID_VS
  #define USE_VERTEX_LIGHTING
#endif


// use the per instance color for the following platforms (for now)
#ifdef _VISION_DX11
  #define USE_PERINSTANCECOLOR
#endif
#ifdef _VISION_DX9
  #define USE_PERINSTANCECOLOR
#endif


#ifdef _VISION_DX11

cbuffer g_GlobalConstantBufferFrame : register (b0)
{
  float4x4 matP              : packoffset(c0);   // projection matrix
  float4x4 matEye            : packoffset(c4);   // eye matrix
  float4x4 matInvEye         : packoffset(c8);   // inverse eye matrix
  float4   cameraPlane       : packoffset(c15);  // camera plane
  float4   timer             : packoffset(c16);  // Time: timer, time difference
  float4   depthFog          : packoffset(c19);  // linear depth fog parameter
  float4   depthFogCol       : packoffset(c20);  // linear depth fog color
  float4   contextCP         : packoffset(c12);  // context clip planes: near, far, 1/(far-near)
  float4   gAmbientCol       : packoffset(c21);  // global ambient color
  float4   lightmapMul       : packoffset(c22);  // lightmap multiplier (skalar) 
}
cbuffer g_GlobalConstantBufferObject : register (b1)
{
  //float4x4 matMV             : packoffset(c0);   // model view matrix. NOTE: Does not make sense here
  float4x4 matMVP            : packoffset(c4);   // model view projection matrix
  float4x4 matM2WS         	 : packoffset(c8); // model to world space matrix
  float4   Sector2Lightmap   : packoffset(c13); // standard model 2 lm register
}

#ifdef _IS_VERTEX_SHADER
cbuffer g_GlobalConstantBufferUser : register (b2)
{
  float   fBaseZ;
  float3  WindPhaseParams;
#ifdef EXPOSE_VEGETATIONPARAMS
  float2 FadeDistances; // x=start y=end
  float ApplyConstraint;
  float WindStrength;
#endif
  float Brightness;

#if defined(DEPTH_BIAS)
  float4 DepthBias;
#endif
}
#endif // _IS_VERTEX_SHADER

cbuffer g_GlobalConstantBufferLightGrid : register (b3)
{
  float4   lightgrid[6]      : packoffset(c0);  // lightgrid (tracking optional)
}
cbuffer g_ConstantBufferTerrainGlobals : register (b5)
{
  float4  sphereConstraint0  : packoffset(c0); // use reserved registers between [0..64]
  float4  sphereConstraint1  : packoffset(c1); 
  float4  WorldSpace2Sector  : packoffset(c2); // transforms world space to sector 0..1 range
}

#ifndef EXPOSE_VEGETATIONPARAMS
cbuffer g_ConstantBufferVegetationModel : register (b6)
{
  float4  VegetationParams   : packoffset(c0); // x:fade start y:1/fade interval z=constraint w=wind
}

#endif // EXPOSE_VEGETATIONPARAMS

// for non-instanced mode
#ifndef USE_INSTANCING
cbuffer g_ConstantBufferVegetationInstance : register (b7)
{
  float4  PerInstanceColor   : packoffset(c0);
}
#endif

#ifdef _IS_PIXEL_SHADER
cbuffer g_GlobalConstantBufferUserPS : register (b2)
{
  float3 MaterialAmbient     : packoffset(c0);
  float4 AlphaThreshold : packoffset(c1);
  float4 MaterialParams      : packoffset(c2); // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself!
}
#endif // _IS_PIXEL_SHADER

#else // DX9
  #ifdef _IS_VERTEX_SHADER
  //float4x4 matMV             : register(c0); // NOTE: Does not make sense here
  float4x4 matP              : register(c4);
  float4x4 matMVP            : register(c8);
  float4x4 matEye            : register(c12);
  float4x4 matInvEye         : register(c16);
  float4x4 matM2WS         	 : register(c20); // model to world space matrix
  float4   contextCP         : register(c24);
  float4   cameraPlane       : register(c27);
  float4   timer             : register(c30);
  float4   lightgrid[6]      : register(c32);
  float4   sphereConstraint0 : register(c44); // use reserved registers between [0..64]
  float4   sphereConstraint1 : register(c45); 
  float4   depthFog          : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  #if defined(_VISION_XENON) && defined(USE_INSTANCING)
    float    vertexCount; //Needed by XBOX360 instancing
  #endif
#ifdef EXPOSE_VEGETATIONPARAMS
  float2 FadeDistances       : register(c66); // x:start, y=end
  float ApplyConstraint      : register(c67);
  float WindStrength         : register(c68);
  float    Brightness        : register(c69); // exposed as a parameter
#if defined(DEPTH_BIAS)
  float4   DepthBias         : register(c70);
#endif
#else // EXPOSE_VEGETATIONPARAMS
  float4   VegetationParams  : register(c48); // x:fade start y:1/fade interval z=constraint w=wind
  float    Brightness        : register(c66); // exposed as a parameter
#if defined(DEPTH_BIAS)
  float4   DepthBias         : register(c67);
#endif
#endif // EXPOSE_VEGETATIONPARAMS
  float4   WorldSpace2Sector : register(c49); // transforms world space to sector 0..1 range
  float4   Sector2Lightmap   : register(c60); // standard model 2 lm register
  float    fBaseZ            : register(c64); // exposed as a parameter
  float3   WindPhaseParams   : register(c65); // exposed as a parameter, default is 0.0071,0.0092,0
#ifndef USE_INSTANCING
  float4  PerInstanceColor   : register(c50); // for non-instanced mode. The register 66 is hardcoded on the app side
#endif

#endif // _IS_VERTEX_SHADER

#endif // DX9




struct VS_IN
{
  float3 ObjPos   : POSITION;
  float3 Normal : NORMAL;
#ifdef USE_MESHVERTEXCOLOR
  float3 VertexColor : COLOR;
#endif

#ifdef USE_INSTANCING
#ifdef USE_PERINSTANCECOLOR
  float4 PerInstanceColor : COLOR1;
#endif
#endif

  float2 UV0 : TEXCOORD0;

#ifdef VMATERIAL_NORMALMAP
  float3 Tangent : TEXCOORD2;
#endif
#ifdef USE_INSTANCING
  float4   Row0     : TEXCOORD5;
  float4   Row1     : TEXCOORD6;
  float4   Row2     : TEXCOORD7;
#endif
};

struct VS_OUT
{
#ifdef MSAA
  centroid float4 ProjPos  : SV_Position;
#else
  float4 ProjPos  : SV_Position;
#endif
	
#ifdef DEPTH_RENDERER
  float3 UV0 : TEXCOORD0; // z=depth
  #if defined(DEPTH_BIAS) && defined(_VISION_DX9)
    float4 Pos : TEXCOORD1;
  #endif
#else
#ifdef USE_FOG
  float3 UV0 : TEXCOORD0; // z=fog
#else
  float2 UV0 : TEXCOORD0;
#endif
  
#ifdef USE_LIGHTMAP
  float2 UV1 : TEXCOORD1;
#endif

#ifdef VMATERIAL_NORMALMAP
#ifndef USE_DEFERRED
  float3 Normal  : TEXCOORD2;
#endif
#ifdef USE_DEFERRED
  float4 Tangent : TEXCOORD3;
  #define DEPTH_REG Tangent.w  // register must be shared based on config
#else
  float3 Tangent : TEXCOORD3;
#endif
  float3 BiTangent: TEXCOORD4;
#endif
#if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION)
  float3 EyeDir : TEXCOORD5;
#endif
#ifdef USE_DEFERRED
  float3 Normal : TEXCOORD2;

#ifndef VMATERIAL_NORMALMAP
  float Depth : TEXCOORD3;
  #define DEPTH_REG Depth  // register must be shared based on config
#endif

#endif

  float4 TintColor : TEXCOORD6; // color to tint the diffuse texture color (wind,...)
#ifdef USE_VERTEX_LIGHTING
  #ifdef CLAMP_LIGHTING
    float4 LightingColor : COLOR;
  #else
    float4 LightingColor : TEXCOORD7; // full precision
  #endif
#endif

#endif // DEPTH_RENDERER
};




// setup aliases
#ifdef EXPOSE_VEGETATIONPARAMS
  #define CONSTRAINT_FLEXIBILITY	ApplyConstraint
  #define WIND_STRENGTH				WindStrength
#else
  #define CONSTRAINT_FLEXIBILITY	VegetationParams.z
  #define WIND_STRENGTH				VegetationParams.w
#endif

#ifdef _IS_VERTEX_SHADER

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////////////////////////////

float3 ConstraintPosition(float3 inPos, float4 sphere)
{
  float3 vDiff = inPos-sphere.xyz;
  float fDist = length(vDiff);
  
  // put position on sphere
  float fLerpVal = (fDist>sphere.w) ? 0 : CONSTRAINT_FLEXIBILITY;
  return lerp(inPos, sphere.xyz + (sphere.w/fDist)*vDiff, fLerpVal);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shader
//////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(_VISION_XENON) && defined(USE_INSTANCING)
  VS_OUT vs_main( int i : INDEX )
#else
  VS_OUT vs_main( VS_IN In )
#endif
{
  VS_OUT Out;
  
#ifdef USE_INSTANCING
  float4 InstanceColor;
  float4 MeshVertexColor;
  #if defined(_VISION_XENON)
    VS_IN In;
    
    float3 vObjPos;
    float3 vNormal;
    float2 vUV0;
    #ifdef VMATERIAL_NORMALMAP
      float3 vTangent;
    #endif
    #ifdef USE_INSTANCING
      float4   vRow0;
      float4   vRow1;
      float4   vRow2;
    #endif
    // Need some constructive casting so 360 compiler doesn't optimize incorrectly
    int iVertexCount = (int)vertexCount;
    int iInstanceDataIndex = (int)( (i+0.5f) / iVertexCount );
    int iGeometryVertexIndex = i - (iInstanceDataIndex * iVertexCount);
    asm
    {
      // Fetch the Mesh Geometry Vertex Data
      vfetch vObjPos.xyz, iGeometryVertexIndex, position0;
      vfetch vNormal.xyz, iGeometryVertexIndex, normal;
      #ifdef USE_MESHVERTEXCOLOR
        vfetch MeshVertexColor.xyzw, iGeometryVertexIndex, color0;
      #endif
      vfetch vUV0.xy, iGeometryVertexIndex, texcoord0;
      #ifdef VMATERIAL_NORMALMAP
        vfetch vTangent.xyz, iGeometryVertexIndex, texcoord2;
      #endif
      // Fetch the Instance Data 
      #ifdef USE_INSTANCING
        #ifdef USE_PERINSTANCECOLOR
        vfetch InstanceColor, iInstanceDataIndex, color1;
        #endif

        vfetch vRow0, iInstanceDataIndex, texcoord5;
        vfetch vRow1, iInstanceDataIndex, texcoord6;
        vfetch vRow2, iInstanceDataIndex, texcoord7;
      #endif
    };

    In.ObjPos.xyz = vObjPos;
    In.Normal.xyz = vNormal;

    In.UV0 = vUV0;
    #ifdef VMATERIAL_NORMALMAP
      In.Tangent.xyz = vTangent;
    #endif
    #ifdef USE_INSTANCING
      In.Row0 = vRow0;
      In.Row1 = vRow1;
      In.Row2 = vRow2;
    #endif
  #else // !_VISION_XENON
    #ifdef USE_PERINSTANCECOLOR
      InstanceColor = In.PerInstanceColor;
	  #endif
    #ifdef USE_MESHVERTEXCOLOR
      MeshVertexColor = In.VertexColor;
    #endif
  #endif
  
  float4 worldPos = float4(In.ObjPos, 1.0f);
  float3x3 rotationMat = float3x3(In.Row0.xyz,In.Row1.xyz,In.Row2.xyz);
  worldPos.xyz =  mul( rotationMat,worldPos.xyz);
  worldPos.xyz += float3(In.Row0.w,In.Row1.w,In.Row2.w);
  float3 worldNormal = normalize(mul( rotationMat, In.Normal ));
  float3 vUpDir = normalize(mul( rotationMat, float3(0,0,1) )); // only relevant for grass normal

#else
  float4 InstanceColor = PerInstanceColor;
  float3 worldNormal = normalize(mul( (float3x3)matM2WS, In.Normal ));
  float4 worldPos = mul(matM2WS,  float4(In.ObjPos,1.0) );
  float3 vUpDir = normalize(mul( (float3x3)matM2WS, float3(0,0,1) )); // only relevant for grass normal
#endif


#ifdef USE_WIND
  // add wind
  float fAmpl = (In.ObjPos.z+fBaseZ)*WIND_STRENGTH;
  float fDistort = sin(timer.x*3.f+dot(worldPos.xyz,WindPhaseParams.xyz));
  fDistort *= vUpDir.z; // only apply wind if vegetation is not flattened
  worldPos.xyz += worldNormal*fAmpl*fDistort;
  worldNormal.z -= fDistort*WIND_STRENGTH;
#endif

  // add 2 sphere constraints
#if SPHERE_CONSTRAINT_COUNT>0
  worldPos.xyz = ConstraintPosition(worldPos.xyz, sphereConstraint0);
#endif
#if SPHERE_CONSTRAINT_COUNT>1
  worldPos.xyz = ConstraintPosition(worldPos.xyz, sphereConstraint1);
#endif

  float4 eyePos = mul( matEye, worldPos );
  Out.ProjPos = mul( matP, eyePos );

#if defined(DEPTH_BIAS)
  Out.ProjPos.z += DepthBias.x;
  if (DepthBias.w > 0.0)
  {
    Out.ProjPos.z = (Out.ProjPos.z / Out.ProjPos.w > 0.0) ? Out.ProjPos.z : 0.0;   
  }
#if defined(_VISION_DX9)
  Out.Pos = Out.ProjPos;
#endif
#endif // DEPTH_BIAS

  Out.UV0.xy = In.UV0;
#ifdef USE_FOG
  Out.UV0.z = GetFogCoord(depthFog, dot(cameraPlane, worldPos));
#endif

#ifdef USE_LIGHTMAP
  float2 sectorUV = worldPos.xy * WorldSpace2Sector.xy + WorldSpace2Sector.zw;
  Out.UV1 = sectorUV.xy * Sector2Lightmap.xy + Sector2Lightmap.zw;
#endif
  
#ifdef VMATERIAL_NORMALMAP
  //output normal, tangent and bi-tangent in worldspace
  Out.Normal  = worldNormal;
  Out.Tangent.xyz = normalize( mul((float3x3)matM2WS , In.Tangent.xyz) );
  float fMul = dot(In.Tangent.xyz, In.Tangent.xyz) - 2.0f;
  Out.BiTangent = normalize( cross(Out.Tangent.xyz, Out.Normal)*fMul );
#endif // VMATERIAL_NORMALMAP

#if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_CUBE_REFLECTION)
  Out.EyeDir = eyePos-worldPos.xyz;
#endif

  float fWindBrightness = 1.0f;
  float4 accum = float4(1,1,1,1);
  
#ifdef USE_LIGHTGRID_VS
    // lightgrid lighting
  float fDot;
  fDot = worldNormal.x*worldNormal.x;
  accum = (worldNormal.x>0) ? (lightgrid[0]*fDot) : (lightgrid[1]*fDot);
  fDot = worldNormal.y*worldNormal.y;
  accum += (worldNormal.y>0) ? (lightgrid[2]*fDot) : (lightgrid[3]*fDot);
  fDot = worldNormal.z*worldNormal.z;
  accum += (worldNormal.z>0) ? (lightgrid[4]*fDot) : (lightgrid[5]*fDot);   
#endif // USE_LIGHTGRID_VS
  
#ifdef USE_WIND
  fWindBrightness = 0.95 + fDistort*0.05; // brightness variation caused by wind
#endif
 
  // fade color:
  float fLODScale = contextCP.w;
  float fDist = length(eyePos.xyz) * fLODScale;// eyePos.z is faster but does not match entity far clip in the engine
  // FadeParams.z is actually 1.0/(fEnd-fStart)
#ifdef EXPOSE_VEGETATIONPARAMS
  float fWeight = (fDist-FadeDistances.x) / (FadeDistances.y - FadeDistances.x);
#else
  float fWeight = (fDist-VegetationParams.x)*VegetationParams.y;
#endif

#ifdef USE_DEFERRED
  Out.DEPTH_REG = eyePos.z/contextCP.y;
  Out.Normal  = worldNormal;
#endif

#ifdef DEPTH_RENDERER
  Out.UV0.z = Out.ProjPos.z / contextCP.y;
#else
  Out.TintColor = float4(fWindBrightness,fWindBrightness,fWindBrightness, saturate(1-fWeight) );
  
  #ifdef USE_PERINSTANCECOLOR
  Out.TintColor *= InstanceColor;
  #endif
 
  #ifdef USE_MESHVERTEXCOLOR
  Out.TintColor *= MeshVertexColor;
  #endif
  
  #ifdef USE_VERTEX_LIGHTING
  Out.LightingColor = accum;
  #endif
 
#endif

  return Out;
}

#endif // _IS_VERTEX_SHADER


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shader
//////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _IS_PIXEL_SHADER

#ifdef _VISION_DX11

  cbuffer g_GlobalConstantBufferLightGrid : register (b3)
  {
    float4      Light0           : packoffset(c0);
    float4      Light1           : packoffset(c1);
    float4      Light2           : packoffset(c2);
    float4      Light3           : packoffset(c3);
    float4      Light4           : packoffset(c4);
    float4      Light5           : packoffset(c5);
  }
  
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
  Texture2D <float4> Lightmap           : register(t1);
  sampler            LightmapSampler    : register(s1);
  Texture2D          NormalMap          : register(t2);
  sampler            NormalMapSampler   : register(s2);
  Texture2D          RefMap             : register(t3);
  sampler            RefMapSampler      : register(s3);
#else // DX9

  float4   lightmapMul       : register(c0);
  float4   depthFogCol       : register(c1);

  float3 gAmbientCol         : register(c21);
  float3 MaterialAmbient     : register(c33);
  float4 AlphaThreshold : register(c34);

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
  float4 MaterialParams : register(c32);
#else
  float4 MaterialParams;
#endif
  float4 Light0 : register(c26);
  float4 Light1 : register(c27);
  float4 Light2 : register(c28);
  float4 Light3 : register(c29);
  float4 Light4 : register(c30);
  float4 Light5 : register(c31);

  sampler2D          BaseTexture        : register(s0);
  sampler2D          Lightmap           : register(s1);
  sampler2D          NormalMap          : register(s2);
  sampler2D          RefMap             : register(s3);
#endif // _VISION_DX11


#ifdef USE_DEFERRED

// deferred
PS_OUT ps_main( VS_OUT In )
{
  PixelDataUnpacked pixelData;
    
  float4 cBaseTex = vTex2D(BaseTexture, BaseTextureSampler, In.UV0.xy);
  cBaseTex *= In.TintColor;
  clip(cBaseTex.a - AlphaThreshold.x);
    
#ifdef USE_VERTEX_LIGHTING
  float4 vBaseLighting = In.LightingColor;
#else
  float4 vBaseLighting = float4(0,0,0,0);
#endif


  float3 vWorldNormal = In.Normal;

#ifdef VMATERIAL_NORMALMAP
  float4 vNormalMap = DecodeNormalAndAlpha(float4(vTex2D(NormalMap, NormalMapSampler, In.UV0.xy).xyz, 1.0f));
  vNormalMap.xyz = normalize(vNormalMap.xyz);
  vWorldNormal = GetWorldNormal(vNormalMap, In.Tangent.xyz, In.BiTangent, vWorldNormal);
#endif

  pixelData.vDiffColor = cBaseTex.xyz;
  pixelData.vAccInitial = cBaseTex.xyz * (vBaseLighting + gAmbientCol + MaterialAmbient);
  pixelData.fDepth = In.DEPTH_REG;
  pixelData.fSpecExp = 1.0f;
  pixelData.vSpecColor = float3(0.0f, 0.0f, 0.0f);
  pixelData.vNormal = vWorldNormal;
  pixelData.fMaterial = 1.0f;
	
  PS_OUT result = packPixelValues(pixelData);

  #ifdef MSAA	
    float edgeResponse = dot(abs(frac(In.ProjPos.xy) - 0.5), 1000000);
    result.Accumulation.a = min (edgeResponse, 0.9);
  #endif

  return result;
}

#else // USE_DEFERRED


// non-deferred
float4 ps_main( VS_OUT In ) : SV_Target
{
  float4 result = vTex2D(BaseTexture, BaseTextureSampler, In.UV0.xy);
#ifdef DEPTH_RENDERER
  // alpha test (hardcoded threshold)
  clip(result.a - 0.25);
  return float4(In.UV0.z, 1.0f, 1.0f, 1.0f);
#else

#ifdef _VISION_DX11
  // alpha test (hardcoded threshold)
  clip(result.a - 0.25);
#endif 


#ifdef USE_LIGHTMAP
  float4 lightmap = vTex2D(Lightmap, LightmapSampler, In.UV1.xy) * lightmapMul.x;
  result.xyz *= lightmap.xyz;
#endif

#ifdef VMATERIAL_NORMALMAP
  float4 cNormalMap = DecodeNormalAndAlpha(vTex2D(NormalMap, NormalMapSampler, In.UV0.xy));
  float3 worldNormal = GetWorldNormal(cNormalMap, In.Tangent.xyz, In.BiTangent, In.Normal);


  #if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_CUBE_REFLECTION)
    float3 eyeNorm = normalize(In.EyeDir);

    #ifdef VMATERIAL_REFMAP
      float4 SpecMul = vTex2D(RefMap, RefMapSampler, In.UV0.xy) * MaterialParams.xxxy;    //specmul masked by specmap
      float fSpecExp = SpecMul.a;
    #else
      float SpecMul = MaterialParams.x;
      #ifdef VMATERIAL_NORMALMAP
      SpecMul *= cNormalMap.w; // specmul masked by normalmap.a
      #endif
      float fSpecExp = MaterialParams.y;
    #endif
  #endif

  #if defined(VMATERIAL_SPECULAR)
    LIGHT_RESULT_SPEC res = GetLightGridSpec_WS(eyeNorm, worldNormal, 
                                         Light0, Light1, 
                                         Light2, Light3, 
                                         Light4, Light5, 
                                         fSpecExp);

#ifdef CLAMP_LIGHTING
    res.diffuse.xyz = saturate(res.diffuse.xyz);
#endif
    result.xyz *= res.diffuse.xyz;
    result.xyz += res.specular.xyz*SpecMul.xyz;

  #else
    // diffuse only
  float4 vDiff = GetLightGridDiffuse_WS( worldNormal,
                                        Light0, Light1,
                                        Light2, Light3,
                                        Light4, Light5 );
#ifdef CLAMP_LIGHTING
    vDiff.xyz = saturate(vDiff.xyz);
#endif
   result.xyz *= vDiff.xyz;
  #endif // VMATERIAL_SPECULAR
#endif // VMATERIAL_NORMALMAP

  result *= In.TintColor;
#ifdef USE_VERTEXCOLOR
  result *= In.LightingColor;
#endif

#ifdef USE_FOG
  result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, In.UV0.z);
#endif

#endif // DEPTH_RENDERER

 return result;
}



#endif // USE_DEFERRED


#endif // _IS_PIXEL_SHADER
