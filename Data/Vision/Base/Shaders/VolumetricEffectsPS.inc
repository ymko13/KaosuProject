//
#include <Shaders/VisionCommon.inc>

$ifdef USE_TEXTURE
  #ifdef _VISION_DX11
    Texture2D   BaseTexture : register(t0);
    sampler     BaseTextureSampler : register(s0);
  #else
    sampler2D   BaseTexture : register(s0);   
  #endif
$endif

struct PS_IN
{
  float4 ProjPos : SV_Position;
  half4 Color : TEXCOORD0;
$ifdef USE_TEXTURE
  half2 UV0 : TEXCOORD1;
$endif
};

half4 ps_main(PS_IN In) : SV_Target
{
  half4 outputColor = In.Color; 
  
$ifdef USE_TEXTURE
  half4 textureColor = (half4)vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
  outputColor *= textureColor;
$endif
  
  return outputColor;
}

