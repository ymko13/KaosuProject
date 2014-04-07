//
#include <Shaders/VisionCommon.inc>

//-----------------------------------------------------------------------------------
// Constants

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMVP  : packoffset(c4); // model view projection matrix
  }
#else
  float4x4 matMVP  : register(c8);
#endif

#ifdef USE_FOG
  #include <Shaders/ShaderHelpers.inc>
  #ifdef _VISION_DX11
    cbuffer g_GlobalConstantBufferFrame : register (b0)
    {
      float4   depthFog : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
    }
  #else
    float4   depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  #endif
#endif

//-----------------------------------------------------------------------------------

struct VS_IN                                   
{                                             
  float3 ObjPos  : POSITION; // Object space position 
  #ifdef USE_COLOR
    float4 Color : COLOR;
  #endif
  #if defined(USE_TEXTURE) || defined(USE_LUMINANCE_ALPHA)
    float2 UV0   : TEXCOORD0;
  #endif
  #ifdef USE_LIGHTMAP
    float2 UV1   : TEXCOORD1;
  #endif
};                                          
                                              
struct VS_OUT                                 
{                                             
  float4   ProjPos     : SV_Position; // Projected space position 
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
                                               
VS_OUT vs_main( VS_IN In )                        
{                                            
  VS_OUT Out;         
  Out.ProjPos = mul(matMVP, float4(In.ObjPos, 1.0f)); 
#ifdef USE_COLOR
  Out.Color = In.Color;
#endif
#if defined(USE_TEXTURE) || defined(USE_LUMINANCE_ALPHA)
  Out.UV0_FogCoord = half3(In.UV0, 0.0h);
#endif
#ifdef USE_LIGHTMAP
  Out.UV1 = In.UV0;
#endif
#ifdef USE_FOG
  Out.UV0_FogCoord.z = GetFogCoord(depthFog, Out.ProjPos.z);
#endif
#ifdef USE_CLIP
  Out.ScreenPos = half2(Out.ProjPos.x, -Out.ProjPos.y);
#endif
  return Out;                              
}

//-----------------------------------------------------------------------------------
