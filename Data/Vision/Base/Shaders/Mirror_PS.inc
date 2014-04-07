// Includes
#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <Shaders/TextureSamplingHelpers.inc>

/////////////////////////////////////////////////////////////////////////////
// Pixel shader sources
/////////////////////////////////////////////////////////////////////////////
#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    depthFogCol     : packoffset(c20);  // linear depth fog color
  }
  
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
  Texture2D <float4> NormalMap          : register(t1);
  sampler            NormalMapSampler   : register(s1);
  Texture2D <float4> BaseTexture2 	    : register(t2);
  sampler            BaseTextureSampler2 : register(s2);
  Texture2D <float4> NormalMap2         : register(t3);
  sampler            NormalMapSampler2  : register(s3);
  Texture2D <float4> ShadowMap          : register(t4);
  sampler            ShadowMapSampler   : register(s4);
#else
  float4 depthFogCol : register(c1);  // linear depth fog color
  sampler2D          BaseTexture        : register(s0);
  sampler2D          NormalMap          : register(s1);
  sampler2D BaseTexture2 : register(s2);
  sampler2D NormalMap2 : register(s3);
  sampler2D ShadowMap : register(s4);
#endif


#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
  float  SmoothRadius    : register(c32);
  float4 ReflectionColor : register(c33);
  float4 BumpIntensity   : register(c34);
  float4 SpecColor : register(c35);
  float4 SpecColor2 : register(c36);
  float4 SpecExp : register(c37);
  float4 TexSize : register(c38);
#elif _VISION_DX11

cbuffer g_GlobalConstantBufferUserPS : register (b2)
{
  float  SmoothRadius    : packoffset(c0);
  float4 ReflectionColor : packoffset(c1);
  float4 BumpIntensity   : packoffset(c2);
  float4 SpecColor : packoffset(c3);
  float4 SpecColor2 : packoffset(c4);
  float4 SpecExp : packoffset(c5);
  float4 TexSize : packoffset(c6);
}  
#else
  float  SmoothRadius;
  float4 ReflectionColor;
  float4 BumpIntensity;
  float4 SpecColor;
  float4 SpecColor2;
  float4 SpecExp;
  float4 TexSize;
#endif

struct PS_IN
{                  
  float4 ProjPos : SV_Position;  
  float4 UVProj  : TEXCOORD0; 
  float3 UVBaseAndFog  : TEXCOORD1;
};

struct PS_IN_SPEC
{
  float4 ProjPos : SV_Position;  
  float4 UVProj    : TEXCOORD0;
  float3 UVBaseAndFog  : TEXCOORD1;
  float2 UVNorm    : TEXCOORD2;
  float2 UVNorm2   : TEXCOORD3;
  float3 vCamDir   : TEXCOORD4;
  float3 vSpecDir  : TEXCOORD5;
  float3 vSpecDir2 : TEXCOORD6;
};

// Bilinear floating point texture sampling not supported on Xbox360
#ifdef HDR_ON_XBOX
  #define SampleMirrorTexture(tex, sampler, texcoords) \
    SampleBilinear(tex, sampler, texcoords * TexSize.xy, TexSize.zw)
#else
  #define SampleMirrorTexture(tex, sampler, texcoords) \
    vTex2D(tex, sampler, texcoords)
#endif

// very simple reflection
float4 ps_main( PS_IN In ) : SV_Target
{
  float4 coord = In.UVProj / In.UVProj.w;
  float4 final = SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy) * ReflectionColor;
  final.xyz = GetFogColor(final.xyz, depthFogCol.xyz, In.UVBaseAndFog.z);  

  final.w = ReflectionColor.w;
  return final;
}

// simple reflection with smoothing
float4 ps_main_smooth( PS_IN In ) : SV_Target
{
  float4 coord = In.UVProj / In.UVProj.w;

  float2 dx = float2(SmoothRadius,0.0);
  float2 dy = float2(0.0,SmoothRadius);
  float4 smoothVal = SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy);
  smoothVal += SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy + dx);
  smoothVal += SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy - dx);
  smoothVal += SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy + dy);
  smoothVal += SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy - dy);
  float4 final = smoothVal * 0.2 * ReflectionColor;
  final.xyz = GetFogColor(final.xyz, depthFogCol.xyz, In.UVBaseAndFog.z);  
  final.w = ReflectionColor.w;
  return final;
}

// reflection with normal map perturbation
float4 ps_main_bump( PS_IN In ) : SV_Target
{
  float4 coord = In.UVProj / In.UVProj.w;
  float4 offset = vTex2D(NormalMap, NormalMapSampler, In.UVBaseAndFog.xy);
  offset -= float4(0.5f, 0.5f, 0.5f, 0.0f);
  coord += offset * BumpIntensity;
  float4 final = SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord.xy) * ReflectionColor;
  final.xyz = GetFogColor(final.xyz, depthFogCol.xyz, In.UVBaseAndFog.z);
  final.w = ReflectionColor.w;
  return final;
}

// reflection with 2x normal map perturbation and 2x specular light
float4 ps_main_spec( PS_IN_SPEC In ) : SV_Target
{
  float4 coord = In.UVProj / In.UVProj.w;
  float4 offset = vTex2D(NormalMap, NormalMapSampler, In.UVNorm);
  offset -= float4(0.5f, 0.5f, 0.5f, 0.0f);
  float4 offset2 = vTex2D(NormalMap2, NormalMapSampler2, In.UVNorm2);
  offset2 -= float4(0.5f, 0.5f, 0.5f, 0.0f);
  float4 finalOffset = offset * BumpIntensity.x + offset2 * BumpIntensity.y;
  float4 refColor = SampleMirrorTexture(BaseTexture, BaseTextureSampler, coord + finalOffset.xy) * ReflectionColor;
  //float4 baseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
  float4 baseColor = vTex2D(BaseTexture2, BaseTextureSampler2, In.UVBaseAndFog.xy);
  float4 finalColor = refColor;//lerp(baseColor, refColor, ReflectionColor.a);
  // camera pos
  float3 vCamDir = normalize(In.vCamDir);
  // specular
  float3 vNormal;
  vNormal.xy = finalOffset.xy * BumpIntensity.z;
  vNormal.z = sqrt(1.0 - vNormal.x * vNormal.x - vNormal.y * vNormal.y);
  float3 vHalf = normalize(In.vSpecDir + vCamDir);
  float fAngle = dot(vHalf, vNormal);
  // specular 2
  float3 vNormal2;
  vNormal2.xy = finalOffset.xy * BumpIntensity.w;
  vNormal2.z = sqrt(1.0 - vNormal2.x * vNormal2.x - vNormal2.y * vNormal2.y);
  float3 vHalf2 = normalize(In.vSpecDir2 + vCamDir);
  float fAngle2 = dot(vHalf2, vNormal2);
  // final combination
  float4 finalSpecColor = SpecColor * pow(fAngle, SpecExp.x) + SpecColor2 * pow(fAngle2, SpecExp.y);
  float4 cShadow = vTex2D(ShadowMap, ShadowMapSampler, In.UVBaseAndFog.xy);
  float4 final = finalColor + finalSpecColor * cShadow;
  final.xyz = GetFogColor(final.xyz, depthFogCol.xyz, In.UVBaseAndFog.z);  
  final.a = baseColor.a;//(baseColor.a + finalSpecColor.a) * cShadow.r * baseColor.a;
  return final;
}