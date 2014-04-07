// Includes
#include <Shaders/VisionCommon.inc>

/////////////////////////////////////////////////////////////////////////////
// Vertex shader sources
/////////////////////////////////////////////////////////////////////////////
#ifdef _VISION_DX11
cbuffer g_GlobalConstantBufferFrame : register (b0)
{
  float4x4 WorldMatrix : packoffset(c4);
  float4   timer      : packoffset(c16);  // Time: timer, time difference
  float4   depthFog   : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
}
cbuffer g_GlobalConstantBufferObject : register (b1)
{
  float4x4 matMVP     : packoffset(c4);   // model view projection matrix
  float4x4 matMV     : packoffset(c0);   // model view matrix
}
#else
  float4x4 matMV     : register(c0);
  float4x4 matMVP     : register(c8);
  float4x4 WorldMatrix : register(c12);
  float4   timer      : register(c30);
  float4   depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
  float4   refPlaneS  : register(c64);
  float4   refPlaneT  : register(c65);
  float4   refPlaneQ  : register(c66);
  float4   mmv0       : register(c67);
  float4   mmv1       : register(c68);
  float4   mmv2       : register(c69);
  float4   mmvpos     : register(c70);
  float4   BumpScroll : register(c71);
  float4   Tiling     : register(c72);
  float4 SpecLightDir : register(c73);
  float4 SpecLightDir2 : register(c74);
#elif defined(_VISION_DX11)
cbuffer g_GlobalConstantBufferUserVS : register (b2)
{
  float4   refPlaneS  : packoffset(c0);
  float4   refPlaneT  : packoffset(c1);
  float4   refPlaneQ  : packoffset(c2);
  float4   mmv0       : packoffset(c3);
  float4   mmv1       : packoffset(c4);
  float4   mmv2       : packoffset(c5);
  float4   mmvpos     : packoffset(c6);
  float4   BumpScroll : packoffset(c7);
  float4   Tiling     : packoffset(c8);
  float4 SpecLightDir : packoffset(c9);
  float4 SpecLightDir2 : packoffset(c10);
}

#else
  float4   refPlaneS;
  float4   refPlaneT;
  float4   refPlaneQ;
  float4   mmv0;
  float4   mmv1;
  float4   mmv2;
  float4   mmvpos;
  float4   BumpScroll;
  float4   Tiling;
  float4 SpecLightDir;
  float4 SpecLightDir2;
#endif

struct VS_IN
{
  float3   ObjPos     : POSITION;
  float2   UVBase     : TEXCOORD0;
};

struct VS_OUT                                 
{                                             
  float4   ProjPos    : SV_Position;              
  float4   UVProj     : TEXCOORD0;
  float3   UVBaseAndFog  : TEXCOORD1;
};                                             

struct VS_IN_SPEC
{
  float3 ObjPos  : POSITION;
  float3 Normal  : NORMAL;
  float2 UVBase  : TEXCOORD0;
  float3 Tangent : TEXCOORD2;
};

struct VS_OUT_SPEC
{
  float4 ProjPos   : SV_Position;
  float4 UVProj    : TEXCOORD0;
  float3 UVBaseAndFog    : TEXCOORD1;
  float2 UVNorm    : TEXCOORD2;
  float2 UVNorm2   : TEXCOORD3;
  float3 vCamDir   : TEXCOORD4;
  float3 vSpecDir  : TEXCOORD5;
  float3 vSpecDir2 : TEXCOORD6;
};

#include <Shaders/ShaderHelpers.inc>

// helper function to compute texture coordinates of reflected render target.
float4 GetProjectedTexCoords(float3 inPos)
{
  float4 eyePos;
  float4 InPos4 = float4(inPos, 1.0f);
  eyePos.x = dot(InPos4, mmv0);
  eyePos.y = dot(InPos4, mmv1);
  eyePos.z = dot(InPos4, mmv2);
  eyePos.w = 1.0f;
  eyePos += mmvpos;

  float4 outUV;
  outUV.w = dot(refPlaneQ, eyePos);
  outUV.x = dot(refPlaneS, eyePos)*0.5f + 0.5f*outUV.w;
  outUV.y = -dot(refPlaneT, eyePos)*0.5f + 0.5f*outUV.w;
  outUV.z = 0.0f;
  return outUV;
}

// base vertex shader
VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
  Out.ProjPos = mul( matMVP, float4(In.ObjPos, 1.0f) );
  Out.UVProj = GetProjectedTexCoords(In.ObjPos);
  
  // forward base coordinates for bumpmap
  Out.UVBaseAndFog.xy = In.UVBase;
  Out.UVBaseAndFog.z = GetFogCoord(depthFog, Out.ProjPos.z);

  return Out;
}

// base vertex shader with scrolling
VS_OUT vs_main_scroll( VS_IN In )
{                     
  VS_OUT Out;
  Out.ProjPos = mul( matMVP, float4(In.ObjPos, 1.0f) );
  Out.UVProj = GetProjectedTexCoords(In.ObjPos);
  
  // forward base coordinates for bumpmap
  Out.UVBaseAndFog.xy = (In.UVBase + timer.xx*BumpScroll) * Tiling;
  Out.UVBaseAndFog.z = GetFogCoord(depthFog, Out.ProjPos.z);
  return Out;
}

// vertex shader with scrolling, 2x bumpmap, 2x specular light
VS_OUT_SPEC vs_main_spec( VS_IN_SPEC In )
{
  VS_OUT_SPEC Out;
  Out.ProjPos = mul( matMVP, float4(In.ObjPos, 1.0f) );
  Out.UVProj = GetProjectedTexCoords(In.ObjPos);
  
  Out.UVBaseAndFog.xy  = In.UVBase;
  Out.UVBaseAndFog.z = GetFogCoord(depthFog, Out.ProjPos.z);
  Out.UVNorm  = (In.UVBase + timer.xx * BumpScroll.xy) * Tiling.xy;
  Out.UVNorm2 = (In.UVBase + timer.xx * BumpScroll.zw) * Tiling.zw;
  
  float3 normal = mul((float3x3)matMV, In.Normal);
  float3 tangent = mul((float3x3)matMV, In.Tangent);
  float fMul = sign(dot(tangent, tangent) - 2.0f);
  normal = normalize(normal);
  tangent = normalize(tangent);
  float3 biNormal = cross(tangent, normal) * fMul;

  float4 mvv = -mul(matMV, float4(In.ObjPos, 1.0f));
  Out.vCamDir = GetVectorInTextureSpace(mvv.xyz, tangent, normal, biNormal);
  float3 vSpecLight = normalize(mul((float3x3)WorldMatrix, SpecLightDir.xyz));
  Out.vSpecDir = GetVectorInTextureSpace(vSpecLight, tangent, normal, biNormal);
  float3 vSpecLight2 = normalize(mul((float3x3)WorldMatrix, SpecLightDir2.xyz));
  Out.vSpecDir2 = GetVectorInTextureSpace(vSpecLight2, tangent, normal, biNormal);  
  return Out;
}
