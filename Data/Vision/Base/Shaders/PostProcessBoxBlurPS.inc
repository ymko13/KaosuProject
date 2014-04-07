//
#include <Shaders/VisionCommon.inc>

//-----------------------------------------------------------------------------------
// Textures

#ifdef _VISION_DX10
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
#else
  sampler2D          BaseTexture        : register(s0);
#endif

//-----------------------------------------------------------------------------------
// Constants

#ifdef POINT_SAMPLING

  #if defined(_VISION_PS3) || defined(_VISION_PSP2)|| defined(_VISION_WIIU) || defined(_VISION_GLES2)
    float4 StepSize : register(c64);
  #elif defined(_VISION_DX11)
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
      float4 StepSize : packoffset(c1);
    }
  #else
    float4 StepSize;
  #endif
#endif

//-----------------------------------------------------------------------------------

struct PS_IN
{                  
  float4 ProjPos : SV_Position;      
  half2 UV0     : TEXCOORD0;
#if !defined(POINT_SAMPLING)
  half2 UV1     : TEXCOORD1;
  half2 UV2     : TEXCOORD2;
#endif
};

half4 ps_main( PS_IN In ) : SV_Target
{
#ifdef POINT_SAMPLING
  // point (nearest) sampling
  float4 color = vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0 + StepSize);
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0 - StepSize);
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0 + 2.0f*StepSize);
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0 - 2.0f*StepSize);
  color *= 0.2f;
#else
  // bilinear sampling: sample in between the outer pixels
  half4 color = vTex2D(BaseTexture, BaseTextureSampler, In.UV1);
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV2);
  color *= 2.0f; // 2 pixels sampled at a time
  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
  color *= 0.2f;
#endif
  return color;
}
