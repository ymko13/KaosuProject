//
#include <Shaders/VisionCommon.inc>
#ifdef USE_FOG
#include <Shaders/ShaderHelpers.inc>
#endif

#ifdef EMULATE_BILINEAR_FILTERING
#include <Shaders/TextureSamplingHelpers.inc>
#define SampleTexture(texture, sampler, uv, invTexSize) SampleBilinear(texture, sampler, uv / invTexSize, invTexSize)
#else
#define SampleTexture(texture, sampler, uv, invTexSize) vTex2D(texture, sampler, uv)
#endif

//-----------------------------------------------------------------------------------
// Textures / Constants

#ifdef _VISION_DX11
  // Textures
  #if defined(USE_TEXTURE) || defined(USE_LUMINANCE_ALPHA)
    Texture2D <float4> BaseTexture        : register(t0);
    sampler            BaseTextureSampler : register(s0);
  #endif
  #ifdef USE_LIGHTMAP
    Texture2D <float4> Lightmap           : register(t1);
    sampler            LightmapSampler    : register(s1);
  #endif

  // Constants
  cbuffer g_GlobalConstantBufferFrame : register(b0)
  {
  #ifdef USE_FOG
    float4 depthFogCol : packoffset(c20);  // linear depth fog color
  #endif
  #ifdef USE_LIGHTMAP
    float4 lightmapMul : packoffset(c22);  // lightmap multiplier (scalar) 
  #endif
  #ifdef USE_CLIP
    float4 clipPlanes : packoffset(c24);
  #endif
  }
  
  #ifdef EMULATE_BILINEAR_FILTERING
    cbuffer g_GlobalConstantBufferUser : register(b2)
    {
      float2 invTexSize : packoffset(c20);
    }
  #endif
#else
  // Textures
  #if defined(USE_TEXTURE) || defined(USE_LUMINANCE_ALPHA)
    sampler2D          BaseTexture        : register(s0);
  #endif
  #ifdef USE_LIGHTMAP
    sampler2D          Lightmap           : register(s1);
  #endif
  
  // Constants
  #ifdef USE_LIGHTMAP
    float4 lightmapMul : register(c0);
  #endif
  #ifdef USE_FOG 
    float4 depthFogCol : register(c1);  // linear depth fog color
  #endif
  #ifdef USE_CLIP
    float4 clipPlanes : register(c24);
  #endif
  
  #ifdef EMULATE_BILINEAR_FILTERING
    #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
      float2 invTexSize : register(c32);
    #else
      float2 invTexSize;
    #endif
  #endif
#endif

//-----------------------------------------------------------------------------------
                                          
struct PS_IN                                 
{                                             
  float4 ProjPos       : SV_Position; // Projected space position 
  #ifdef USE_COLOR
    half4 Color        : COLOR;
  #endif
  #if defined(USE_TEXTURE) || defined(USE_LUMINANCE_ALPHA)
    half3 UV0_FogCoord : TEXCOORD0;
  #endif
  #ifdef USE_LIGHTMAP
    half2 UV1          : TEXCOORD1;
  #endif
  #ifdef USE_CLIP
    half2 ScreenPos    : TEXCOORD2;
  #endif
};                                             

float4 ps_main( PS_IN In ) : COLOR                 
{
#if defined(USE_CLIP)
  half4 test = In.ScreenPos.xyxy - clipPlanes;
  clip(min(min(test.x, -test.z), min(test.y, -test.w)));
#endif

#if defined(USE_TEXTURE) && defined(USE_COLOR)
  half4 result = SampleTexture(BaseTexture, BaseTextureSampler, In.UV0_FogCoord.xy, invTexSize) * In.Color;
#elif defined(USE_TEXTURE)
  half4 result = SampleTexture(BaseTexture, BaseTextureSampler, In.UV0_FogCoord.xy, invTexSize);
#elif defined(USE_COLOR)
  half4 result = In.Color;
#else
  half4 result = half4(1.0h, 1.0h, 1.0h, 1.0h);
#endif

#if defined(USE_LUMINANCE_ALPHA)
  result.a *= SampleTexture(BaseTexture, BaseTextureSampler, In.UV0_FogCoord.xy, invTexSize).r;
#endif

#if defined(USE_LIGHTMAP)
  result *= vTex2D(Lightmap, LightmapSampler, In.UV1) * lightmapMul.x;
#endif

#if defined(USE_FOG)
  result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, In.UV0_FogCoord.z);
#endif        
  return result;                              
}

//-----------------------------------------------------------------------------------
