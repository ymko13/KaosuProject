//
#include <Shaders/VisionCommon.inc>

//-----------------------------------------------------------------------------------
// Textures

#ifdef _VISION_DX11
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
#else
  sampler2D          BaseTexture        : register(s0);
#endif

//-----------------------------------------------------------------------------------
// Constants

#if defined(_VISION_PS3) || defined(_VISION_PSP2)|| defined(_VISION_WIIU) || defined(_VISION_GLES2)
  #ifdef USE_GLOW
    // x = bias, y = power, z = scale
    // on mobile:
    // x = bias, y = pow-approximation-factor, z = scale, w = pow-approximation-bias
    float4 GlowParams : register(c33);
  #endif	
  #ifdef POINT_SAMPLING
    float4   StepSize : register(c64);
  #endif
  
#elif defined(_VISION_DX11)
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    #ifdef USE_GLOW
      float4 GlowParams : packoffset(c0);
    #endif 
    #ifdef POINT_SAMPLING
      float4 StepSize : packoffset(c1);
    #endif
  }
#else
  #ifdef USE_GLOW
    float4 GlowParams;
  #endif	
  #ifdef POINT_SAMPLING
    float4 StepSize;
  #endif

#endif

//-----------------------------------------------------------------------------------

struct PS_IN
{                  
  float4 ProjPos  : SV_Position;
  half2 UV0      : TEXCOORD0; 
  
  #if !defined(POINT_SAMPLING)
    #ifdef DOWNSAMPLE_4X4
      half2 UV1     : TEXCOORD1;
      half2 UV2     : TEXCOORD2;
      half2 UV3     : TEXCOORD3;
    #endif
  #endif
};

half4 ps_main(PS_IN In) : SV_Target
{ 
  // downsample 2x2
  half4 color = vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
  
  #ifdef POINT_SAMPLING
    // sample each pixel individually when using point sampling
    color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(0.0h, StepSize.y));
	  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize.x, StepSize.y));
	  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize.x, 0.0h));
    
    #ifdef DOWNSAMPLE_4X4
      half2 StepSize2 = 2.0h * StepSize.xy;
      half2 StepSize3 = 3.0h * StepSize.xy;

		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize2.x, 0.0h));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize3.x, 0.0h));

		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize2.x, StepSize.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize3.x, StepSize.y));

		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(0.0h, StepSize2.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize.x, StepSize2.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize2.x, StepSize2.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize3.x, StepSize2.y));

		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(0.0h, StepSize3.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize.x, StepSize3.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize2.x, StepSize3.y));
		  color += vTex2D(BaseTexture, BaseTextureSampler, In.UV0+half2(StepSize3.x, StepSize3.y));
		  
		  color *= 0.0625h;
	  #else
	    color *= 0.25h;
	  #endif
    
  #else
  
    #ifdef DOWNSAMPLE_4X4
      color += vTex2D(BaseTexture, BaseTextureSampler, In.UV1);
      color += vTex2D(BaseTexture, BaseTextureSampler, In.UV2);
      color += vTex2D(BaseTexture, BaseTextureSampler, In.UV3);
      color *= 0.25h;
    #endif
    
  #endif
      
  #if defined(USE_GLOW)
    color += GlowParams.xxxx;
    half fIntensity = max(0.0h, dot(color.xyz, (half3)0.3333h));
    
    #if defined(_VISION_GLES2)
      // approximate pow function on mobile (quadratic)
      half fTmp = max(GlowParams.y*fIntensity + GlowParams.w, 0.0h);
      fTmp *= fTmp;
      color *= fTmp * GlowParams.z;
    #else
      color *= pow(fIntensity, GlowParams.y) * GlowParams.z;
    #endif
    
    color.a = 1.0h;
    
  #elif !defined(OUTPUT_DEPTH)
    color = clamp(color * 0.1h, 0.0h, 1.0h);
  #endif
  
  return color;  
}
