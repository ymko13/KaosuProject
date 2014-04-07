//
#include <Shaders/VisionCommon.inc>

#ifdef USE_DOF
  #define CLIP_PLANE_NAME focalPar
#else  
  #define CLIP_PLANE_NAME alphaClip
#endif

#ifdef DEPTH_FILL

  #ifdef DEPTH_FILL_TRANSPARENT
    #ifdef _VISION_DX11
      Texture2D <float4> BaseTexture        : register(t0);
      sampler            BaseTextureSampler : register(s0);
    #else
      sampler2D          BaseTexture        : register(s0);
    #endif
  #endif

  #if defined(_VISION_PS3) || defined(_VISION_WIIU)
    float4 CLIP_PLANE_NAME : register(c32);
  #elif defined (_VISION_DX11)
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
      float4 CLIP_PLANE_NAME : packoffset(c0);
    }
  #else
    float4 CLIP_PLANE_NAME;
  #endif
  
#endif



struct PS_IN
{
  float4 ProjPos  : SV_Position;
  
  #ifdef DEPTH_CLEAR
    float4 TargetValues : TEXCOORD0;
  #endif
  
  #ifdef DEPTH_FILL
    float  Depth : TEXCOORD1;
	
	  #ifdef DEPTH_FILL_TRANSPARENT
	    float2 UV0 : TEXCOORD0;
	  #endif
  #endif
};



float4 ps_main( PS_IN In ) : SV_Target
{
  #ifdef DEPTH_CLEAR
    return In.TargetValues;
  #endif

  #ifdef DEPTH_FILL
    #ifdef DEPTH_FILL_TRANSPARENT
	    #ifdef USE_DOF
	      clip(vTex2D(BaseTexture, BaseTextureSampler, In.UV0).a-focalPar.z);
	    #else
	      clip(vTex2D(BaseTexture, BaseTextureSampler, In.UV0).a-alphaClip.x);
      #endif	  
	  #endif
	
	  float blur = 0.0;
	
	  #ifdef USE_DOF
      blur = saturate( abs( In.Depth - focalPar.x ) * focalPar.y );
	  #endif  
	
	  return float4(In.Depth, blur, 0.0f, 1.0f);
  #endif
}




