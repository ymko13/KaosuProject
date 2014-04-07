//
#include <Shaders/VisionCommon.inc>

#ifdef ALPHA_TEST

#ifdef _VISION_DX11
  Texture2D <float4> BaseTex        : register(t0);
  sampler            BaseTexSampler : register(s0);
#else
  sampler2D          BaseTex        : register(s0);
#endif

#if defined(_VISION_DX11)
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    float4 AlphaThreshold;
  }
#elif defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_CAFE) || defined(_VISION_GLES2)
  float4 AlphaThreshold: register(c33);
#else
  float4 AlphaThreshold;
#endif

#endif

struct PS_IN
{
  float4 ProjPos  : SV_Position;
#ifdef ALPHA_TEST
  float2 UV : TEXCOORD0;
#endif

#if defined(_VISION_DX9) || defined( TARGET_DOWNLEVEL_SM40_LEVEL_93 )
  float4 Pos : TEXCOORD1;
#endif
};

float4 main( PS_IN In ) : SV_Target
{
#ifdef ALPHA_TEST
  float baseAlpha = vTex2D(BaseTex, BaseTexSampler, In.UV).a;
  clip(baseAlpha-AlphaThreshold.r);
#endif

#if defined( _VISION_DX9 ) || defined( TARGET_DOWNLEVEL_SM40_LEVEL_93 )
  return In.Pos.z / In.Pos.w;
#else
  return 0.0;
#endif
}



