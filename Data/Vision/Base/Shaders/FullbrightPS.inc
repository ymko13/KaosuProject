//
#include <Shaders\VisionCommon.inc>
#include <Shaders\ShaderHelpers.inc>

#ifdef _VISION_DX11

  Texture2D BaseTexture                : register (t0);
  sampler   BaseTextureSampler         : register (s0);

  cbuffer g_GlobalConstantBufferFrame  : register (b0)
  {
    float4    depthFogCol              : packoffset (c20);
  }
  
  $ifdef ALPHATEST
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
      #ifdef ALPHA_DISSOLVE
        float     DissolveAlpha : c0;
      #endif
      float     AlphaThreshold;
    }
  $else
    #ifdef ALPHA_DISSOLVE
      cbuffer g_GlobalConstantBufferUser : register (b2)
      {
        float     DissolveAlpha : c0;
      }
    #endif
  $endif
  
#else

  sampler2D BaseTexture     : register (s0);
  float4    depthFogCol     : register (c1);

  $ifdef ALPHATEST
    #if defined(_VISION_PS3) || defined(_VISION_GLES2) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
      float AlphaThreshold  : register (c32);
    #else
      float AlphaThreshold;  
    #endif
  $endif

  #ifdef ALPHA_DISSOLVE
    float DissolveAlpha;
  #endif
  
#endif

struct PS_IN
{
  float4 ProjPos     : SV_Position;
  float2 UV0         : TEXCOORD0;
	
	$ifdef VERTEXCOLOR
		float4 VertColor   : TEXCOORD1;
	$endif
	
  float  fogCoord  : TEXCOORD2;
};

float4 ps_main( PS_IN In ) : SV_Target
{
  float4 result = vTex2D (BaseTexture, BaseTextureSampler, In.UV0);
  
  $ifdef VERTEXCOLOR
    result *= In.VertColor;
  $endif
  
  $ifdef ALPHATEST
    clip(result.a - AlphaThreshold);
  $endif

  result.xyz = GetFogColor (result.xyz, depthFogCol.xyz, In.fogCoord);

  #ifdef ALPHA_DISSOLVE
    result.a *= DissolveAlpha;
  #endif
  
  return result;
}



