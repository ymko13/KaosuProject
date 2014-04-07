#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

$ifdef VMATERIAL_GLOWMAP
  V_REQUIRES_SM30_SM40L93
$endif

#ifdef _VISION_DX11
  Texture2D BaseTexture             : register(t0);
  sampler   BaseTextureSampler      : register(s0);

  // no dot3 lightmap
  Texture2D   Lightmap              : register(t2);
  sampler     LightmapSampler       : register(s2);

  $ifdef VMATERIAL_GLOWMAP
    Texture2D GlowMap               : register(t3);
    sampler   GlowMapSampler        : register(s3);
  $endif

  #ifdef DISSOLVE
    Texture2D DitheringMask         : register(t7);
    sampler   DitheringMaskSampler  : register(s7);
  #endif

#else
  sampler2D   BaseTexture           : register(s0);
  sampler2D   Lightmap              : register(s2); // no dot3 lightmap

  $ifdef VMATERIAL_GLOWMAP
    sampler2D GlowMap               : register(s3);
  $endif	

  #ifdef DISSOLVE
    sampler2D DitheringMask         : register(s7);
  #endif

#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)

  float4 lightmapMul                : register(c0);
  float4 depthFogCol                : register(c1);  // linear depth fog color
  
  $ifdef ALPHATEST
    float AlphaThreshold       : register(c33);
  $endif

  $ifdef AMBIENT_COLOR
    float vAmbientColor             : register(c21);
    float  GlobalAmbientMultiplier  : register(c34);
  $endif

  $ifdef VMATERIAL_GLOWMAP
    float3 GlowColor                : register(c35);
    float GlowIntensity             : register(c36);
  $endif  
  
#elif defined(_VISION_DX11)

  cbuffer g_GlobalConstantBufferFrame  : register (b0)
  {
    float4 depthFogCol     : packoffset(c20);  // linear depth fog color

    $ifdef AMBIENT_COLOR
      float vAmbientColor  : packoffset(c21);
    $endif

    float4    lightmapMul  : packoffset(c22);  // lightmap multiplier (scalar) 
  }

  cbuffer g_GlobalConstantBufferUser   : register (b2)
  {
    $ifdef ALPHATEST
      float  AlphaThreshold     : packoffset(c0);
    $endif

    $ifdef AMBIENT_COLOR
      float  GlobalAmbientMultiplier : packoffset(c1);
    $endif

    $ifdef VMATERIAL_GLOWMAP
      float3 GlowColor               : packoffset(c2);
      float  GlowIntensity           : packoffset(c3);	
    $endif	

    #ifdef DISSOLVE
      float4 DitheringParams         : packoffset(c7);
    #endif
  }

#else

  float4 lightmapMul          : register(c0);
  float4 depthFogCol          : register(c1);  // linear depth fog color

  $ifdef AMBIENT_COLOR
    float  vAmbientColor      : register(c21);
    float  GlobalAmbientMultiplier;
  $endif

  $ifdef ALPHATEST
    float  AlphaThreshold;
  $endif

  $ifdef VMATERIAL_GLOWMAP
    float3 GlowColor          : register(c32);
    float  GlowIntensity      : register(c33);
  $endif

  #ifdef DISSOLVE
    float4 DitheringParams    : register(c39);
  #endif

#endif

struct PS_IN
{
  #ifndef DISSOLVE
    float4 ProjPos    : SV_Position;
  #else
    float4 ProjPos    : VPOS;
  #endif
  float4 Color        : COLOR0;
  float2 UV0          : TEXCOORD0;  //diffuse
  $ifdef LIGHTMAP  
    float2 UV1        : TEXCOORD1;  //lightmap
  $endif  
  $ifdef FOG
    float fogCoord    : TEXCOORD2;
  $endif
};

float4 GetDiffuseTexture(float2 UV)
{
  return vTex2D(BaseTexture, BaseTextureSampler, UV);
}


float4 ps_main( PS_IN In ) : SV_Target
{
  #ifdef DISSOLVE  // --- Dissolve feature is only used by simulation; see Data/Vision/Simulation/Shaders/vDeferredShadingDissolve.ShaderLib
    // perform 8x8 ordered dithering (LOD dissolve)
    float2 vDitherPos = fmod(In.ProjPos.xy + DitheringParams.zw, 8.0f) / 8.0f;
    float fThreshold = vTex2D(DitheringMask, DitheringMaskSampler, vDitherPos).r;
    clip((fThreshold - DitheringParams.x) * DitheringParams.y);
  #endif

  // sample the textures
  float4 cColor = GetDiffuseTexture(In.UV0);
  cColor.a *= In.Color.a;

  $ifdef ALPHATEST
    clip(cColor.a - AlphaThreshold);
  $endif

  $ifdef LIGHTMAP  
    float3 LightColor = vTex2D(Lightmap, LightmapSampler, In.UV1).xyz * lightmapMul.x + In.Color.rgb;
  $else
    float3 LightColor = In.Color.rgb;
  $endif

  $ifdef AMBIENT_COLOR
    LightColor += vAmbientColor*GlobalAmbientMultiplier;
  $endif

  cColor.xyz *= LightColor;

  $ifdef VMATERIAL_GLOWMAP
    float3 cGlowTex = vTex2D(GlowMap, BaseTextureSampler, In.UV0).rgb;
    cColor.rgb += cGlowTex * GlowColor.rgb * GlowIntensity;
  $endif  

  $ifdef FOG
    cColor.xyz = GetFogColor(cColor.xyz, depthFogCol.xyz, In.fogCoord);
  $endif

  return cColor;
}
