//

#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <Shaders/SkinningVS.inc>

#ifdef _VISION_DX11
cbuffer g_GlobalConstantBufferFrame : register (b0)
{
  float4x4 matP              : packoffset(c0);   // projection matrix
  float4x4 matEye            : packoffset(c4);   // eye matrix
}
cbuffer g_GlobalConstantBufferObject : register (b1)
{
  float4x4 matMV  : packoffset(c0);   // model view matrix
  float4x4 matMVP : packoffset(c4);   // model view projection matrix
}
cbuffer g_CustomConstantBufferTerrain_VS : register (b3)
{
  float4   SectorScaleOfs             : packoffset(c0); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
}
#else
  float4x4 matMV  : register(c0);
  float4x4 matP   : register(c4);
  float4x4 matMVP : register(c8);
  float4x4 matEye : register(c12);
  float4   SectorScaleOfs   : register(c38); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
  #if defined(_VISION_XENON) && defined(INSTANCING)
    float vertexCount; //Needed by XBOX360 instancing (not used when skinning)
  #endif
#endif

#if defined(_VISION_DX11)
cbuffer g_GlobalConstantBufferUser : register (b2)
{
  float4 DepthBias  : packoffset(c0);
}
  
#else
  float4 DepthBias  : register(c64);
  
#endif
  
#ifdef GEOMETRYTYPE_TERRAIN

struct VS_IN
{
  float2 ObjPosXY : POSITION; // shared stream
  float ObjPosZ : TEXCOORD0;  // sector's own stream
};

#else

struct VS_IN
{
  float3 ObjPos : POSITION;
  #ifdef ALPHA_TEST
    float2 UV0 : TEXCOORD0;
  #endif
  SKINNING_VERTEX_STREAMS
#ifdef INSTANCING
  float4   Row0     : TEXCOORD5;
  float4   Row1     : TEXCOORD6;
  float4   Row2     : TEXCOORD7;
#endif
};

#endif //  GEOMETRYTYPE_TERRAIN


struct VS_OUT
{
  float4 ProjPos  : SV_Position;
#ifdef ALPHA_TEST
  float2 UV : TEXCOORD0;
#endif

#if defined(_VISION_DX9) || defined( TARGET_DOWNLEVEL_SM40_LEVEL_93 )
  float4 Pos : TEXCOORD1;
#endif
};


#ifdef GEOMETRYTYPE_TERRAIN

// calculates the world space vertex position
float4 GetVertexPosition(VS_IN In)
{
  float4 pos = float4(In.ObjPosXY.x,In.ObjPosXY.y,In.ObjPosZ,1.0);
  pos.xy = pos.xy*SectorScaleOfs.xy + SectorScaleOfs.zw;
  return pos;
}

#else

float4 GetVertexPosition(VS_IN In)
{
  float4 objPos4 = float4(In.ObjPos, 1.0f);

  #ifdef USE_SKINNING
  PREPARE_SKINNING(In.BoneIndices);
  TRANSFORM_OBJECT_POS(objPos4, In.ObjPos, In.BoneWeight);
  #endif
  
  return (objPos4);
}

#endif //  GEOMETRYTYPE_TERRAIN

#ifndef USE_SKINNING
	#if defined(INSTANCING) && defined(_VISION_XENON)
		VS_OUT main( int i : INDEX )
	#else
		VS_OUT main( VS_IN In )
	#endif
#else
VS_OUT main_skinning( VS_IN In )
#endif
{
  VS_OUT Out;

#if !defined(USE_SKINNING) && defined(INSTANCING) && defined(_VISION_XENON)
    VS_IN In;
    
    float3 vObjPos;
#ifdef ALPHA_TEST
    float2 vUV0;
#endif
	float4   vRow0;
	float4   vRow1;
	float4   vRow2;

    // Need some constructive casting so 360 compiler doesn't optimize incorrectly
    int iVertexCount = (int)vertexCount;
    int iInstanceDataIndex = (int)( (i+0.5f) / iVertexCount );
    int iGeometryVertexIndex = i - (iInstanceDataIndex * iVertexCount);
    asm
    {
		// Fetch the Mesh Geometry Vertex Data
		vfetch vObjPos.xyz, iGeometryVertexIndex, position0;
#ifdef ALPHA_TEST
		vfetch vUV0.xy, iGeometryVertexIndex, texcoord0;
#endif
		vfetch vRow0, iInstanceDataIndex, texcoord5;
		vfetch vRow1, iInstanceDataIndex, texcoord6;
		vfetch vRow2, iInstanceDataIndex, texcoord7;
    };

    In.ObjPos.xyz = vObjPos;
#ifdef ALPHA_TEST
    In.UV0 = vUV0;
#endif
	In.Row0 = vRow0;
	In.Row1 = vRow1;
	In.Row2 = vRow2;
#endif

  float fDepthBias = DepthBias.x;
#ifdef GEOMETRYTYPE_TERRAIN
  fDepthBias *= 8.f; // self shadowing artifacts are very obvious in here otherwise
#endif  

#ifdef INSTANCING
  float4 worldPos = GetVertexPosition(In);
  float3x3 rotationMat = float3x3(In.Row0.xyz,In.Row1.xyz,In.Row2.xyz);
  worldPos.xyz =  mul( rotationMat,worldPos.xyz);
  worldPos.xyz += float3(In.Row0.w,In.Row1.w,In.Row2.w);
  Out.ProjPos = mul( matP, mul( matEye, worldPos ) );
#else
  Out.ProjPos = mul( matMVP, GetVertexPosition(In) );
#endif
  Out.ProjPos.z += fDepthBias;

  if (DepthBias.w > 0.0)
  {
    Out.ProjPos.z = (Out.ProjPos.z / Out.ProjPos.w > 0.0) ? Out.ProjPos.z : 0.0;   
  }
  
  #if defined( _VISION_DX9 ) || defined( TARGET_DOWNLEVEL_SM40_LEVEL_93 )
    Out.Pos = Out.ProjPos;
  #endif

#ifdef ALPHA_TEST
  Out.UV = In.UV0;
#endif

  return Out;
}