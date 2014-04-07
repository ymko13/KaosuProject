//
#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

#ifdef _VISION_DX11
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
#else
  sampler2D          BaseTexture        : register(s0);
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU) || defined(_VISION_GLES2)
  #ifdef USE_COLOR_TRANSFORM
    float4x4 ColorTransform : register(c32);
  #endif
  
  #ifdef USE_MOTIONBLUR
	  float    BlurFactor   : register(c36);
  #endif
#elif defined(_VISION_DX11)
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    #ifdef USE_COLOR_TRANSFORM
      float4x4 ColorTransform : packoffset(c0);
    #endif
  
    #ifdef USE_MOTIONBLUR
	    float    BlurFactor     : packoffset(c4);
    #endif
  }
#else
  #ifdef USE_COLOR_TRANSFORM
    float4x4 ColorTransform;
  #endif
  
  #ifdef USE_MOTIONBLUR
    float    BlurFactor;
  #endif
#endif

struct PS_IN
{                  
  float4 ProjPos  : SV_Position;
  half2 UV0 : TEXCOORD0; 
};

float4 ps_main( PS_IN In ) : SV_Target
{
  float4 result = vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
  
$if defined(GAMMA_CORRECTION) && !defined(GAMMA_CORRECTION_POST_TRANSFORM)
  result.xyz = LinearToSRGB(result.xyz);
$endif

#ifdef USE_COLOR_TRANSFORM
  #ifdef USE_LUMINANCE
    float fLuminance = dot(result.rgb, (float3)0.33333f);
    result /= 1.0f + fLuminance;
  #endif

  result.a = 1.0f;
  result = mul(result, ColorTransform);
#endif
  
$ifdef GAMMA_CORRECTION_POST_TRANSFORM
  result.xyz = LinearToSRGB(result.xyz);
$endif    
  
#ifdef USE_MOTIONBLUR
  result.a = BlurFactor;
#endif
  
  return result;
}
