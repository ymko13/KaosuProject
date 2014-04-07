//
#include <Shaders/VisionCommon.inc>

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMVP  : packoffset(c4);   // model view projection matrix
  }
#else
  float4x4 matMVP  : register(c8);
#endif

// StepSize constant
#if !defined(POINT_SAMPLING) && (defined(BOX_BLUR) || defined(DOWNSAMPLE_2X2) || defined(DOWNSAMPLE_4X4))

  #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU) || defined(_VISION_GLES2)
    float4   StepSize : register(c64);
  #elif defined(_VISION_DX11)
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
      float4   StepSize : packoffset(c0);
    }
  #else
    float4   StepSize;
  #endif

#endif

struct VS_IN
{
  float3   ObjPos  : POSITION;
  #ifdef USE_COLOR
    float4   Color   : COLOR0;  
  #endif
  float2   UV0     : TEXCOORD0;
};

struct VS_OUT
{
  float4   ProjPos : SV_Position;
  #ifdef USE_COLOR
    half4   Color   : COLOR0;
  #endif

  half2   UV0     : TEXCOORD0; 
  
  // Use additional interpolators for mobile in order to get independent texture fetches.
  // No noticeable performance impact on other platforms. (takes advantage of bilinear filtering)
  #if !defined(POINT_SAMPLING)
    #ifdef BOX_BLUR
      half2   UV1     : TEXCOORD1;
      half2   UV2     : TEXCOORD2;
    #endif
    
    #ifdef DOWNSAMPLE_4X4
      half2   UV1     : TEXCOORD1;
      half2   UV2     : TEXCOORD2;
      half2   UV3     : TEXCOORD3;
    #endif
  #endif
};

VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
  Out.ProjPos = mul(matMVP, float4(In.ObjPos, 1.0f));
  
  #ifdef USE_COLOR
    Out.Color = In.Color;
  #endif
  
  #if !defined(POINT_SAMPLING) && (defined(DOWNSAMPLE_2X2) || defined(DOWNSAMPLE4X4))
    Out.UV0 = In.UV0 + StepSize.xy*0.5f;
  #else
    Out.UV0 = In.UV0;
  #endif
  
  #if !defined(POINT_SAMPLING)
    #ifdef BOX_BLUR
      half2 StepSize1_5 = StepSize.xy * 1.5f;
      Out.UV1 = In.UV0 + StepSize1_5;
      Out.UV2 = In.UV0 - StepSize1_5;
    #endif
    #ifdef DOWNSAMPLE_4X4
      half2 StepSize2 = StepSize.xy * 2.0f;
      Out.UV1 = Out.UV0 + half2(StepSize2.x, 0.0f);
      Out.UV2 = Out.UV0 + half2(0.0f, StepSize2.y);
      Out.UV3 = Out.UV0 + StepSize2;
    #endif
  #endif
  
  return Out;
}
