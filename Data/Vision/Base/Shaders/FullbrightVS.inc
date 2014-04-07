//
#include <Shaders\VisionCommon.inc>
#include <Shaders\ShaderHelpers.inc>

#include "SkinningVS.inc"

#ifdef _VISION_DX11

  cbuffer g_GlobalConstantBufferFrame  : register (b0)
  {
    float4   depthFog : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
  }
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMVP   : packoffset(c4);   // model view projection matrix
  }
  
#else

  float4x4 matMVP   : register(c8);
  float4   depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  
#endif

struct VS_IN
{
  float3 ObjPos    : V_VERTEX_STREAM_POSITION;
	
	$ifdef VERTEXCOLOR
		float4 VertColor : V_VERTEX_STREAM_COLOR;
	$endif
	
  float2 UV0       : V_VERTEX_STREAM_BASETEXCOORD;

  SKINNING_VERTEX_STREAMS
};

struct VS_OUT
{
  float4 ProjPos   : SV_Position;
  float2 UV0       : TEXCOORD0;
	
	$ifdef VERTEXCOLOR
		float4 VertColor : TEXCOORD1;
	$endif
	
  float fogCoord   : TEXCOORD2;
};

#ifndef USE_SKINNING

  VS_OUT vs_main (VS_IN In)
  {
    VS_OUT Out;
    Out.ProjPos = mul (matMVP, float4 (In.ObjPos, 1.0f));
    Out.UV0 = In.UV0;
		
		$ifdef VERTEXCOLOR
			Out.VertColor = In.VertColor;
		$endif
		
    Out.fogCoord = GetFogCoord (depthFog, Out.ProjPos.z);

    return Out;
  }

#endif

#ifdef USE_SKINNING

  VS_OUT vs_skinning_main (VS_IN In)
  {
    VS_OUT Out;
    Out.UV0 = In.UV0;
		
		$ifdef VERTEXCOLOR
			Out.VertColor = In.VertColor;
		$endif

    // Skin In.ObjPos
    float4 ObjSpacePos;
    SKIN_NO_NORMAL (ObjSpacePos, In.ObjPos, In.BoneIndices, In.BoneWeight);
    Out.ProjPos = mul (matMVP, ObjSpacePos);
    Out.fogCoord = GetFogCoord (depthFog, Out.ProjPos.z);

    return Out;
  }

#endif