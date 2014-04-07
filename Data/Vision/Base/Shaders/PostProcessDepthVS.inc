//
#include <Shaders/VisionCommon.inc>

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMVP  		: packoffset(c4);   // model view projection matrix
  }
#else
  float4x4 matMVP  		: register(c8);
#endif

#ifdef DEPTH_CLEAR
  #if defined(_VISION_PS3) || defined(_VISION_WIIU)
  
    #ifdef USE_DOF
      float4   focalPar     : register(c64);
	  #endif
	
    float4   clipPlanes   : register(c65);
	
  #elif defined(_VISION_DX11)
  
    cbuffer g_GlobalConstantBufferUser : register (b2)
    {
	    #ifdef USE_DOF
        float4   focalPar     : packoffset(c0);
	    #endif
	  
      float4   clipPlanes   : packoffset(c1);
    }
	
  #else
    #ifdef USE_DOF
      float4   focalPar;
	  #endif
	
    float4   clipPlanes;	
  #endif
#endif

struct VS_IN
{
  float3   ObjPos  : POSITION;
  
  #ifdef DEPTH_FILL_TRANSPARENT
    float2   UV0     : TEXCOORD0;
  #endif
};

struct VS_OUT
{
  float4   ProjPos : SV_Position;
  
  #ifdef DEPTH_FILL
    float    Depth   : TEXCOORD1;
  
    #ifdef DEPTH_FILL_TRANSPARENT
      float2   UV0     : TEXCOORD0;
    #endif
  #endif
  
  #ifdef DEPTH_CLEAR
    float4   TargetValues : TEXCOORD0;
  #endif
};

VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
  Out.ProjPos = mul( matMVP, float4(In.ObjPos, 1.0f) );
  
  #ifdef DEPTH_FILL
    Out.Depth = Out.ProjPos.z;

    #ifdef DEPTH_FILL_TRANSPARENT
	    Out.UV0 = In.UV0;
    #endif
  #endif
  
  #ifdef DEPTH_CLEAR
    float blur = 0.0;
	
	  #ifdef USE_DOF
	    blur = saturate( abs( clipPlanes.y - focalPar.x ) * focalPar.y );
	  #endif
	
    Out.TargetValues = float4(clipPlanes.y, blur, 0.0f, 1.0f);
  #endif
  
  return Out;
}


