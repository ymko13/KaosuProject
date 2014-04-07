#include <Shaders/VisionCommon.inc>
#include <Shaders/SkinningVS.inc>
#include "ShaderHelpers.inc"

struct VS_IN
{
  float3 ObjPos         : V_VERTEX_STREAM_POSITION;
  $ifdef VERTEXCOLOR
    float4 VertColor    : V_VERTEX_STREAM_COLOR;
  $endif
    float3 Normal       : V_VERTEX_STREAM_NORMAL;
    float2 UV0          : V_VERTEX_STREAM_BASETEXCOORD;
  $ifdef LIGHTMAP  
    float2 UV1          : V_VERTEX_STREAM_LIGHTMAPCOORD;
  $endif  
  
  SKINNING_VERTEX_STREAMS
};

struct VS_OUT
{
  float4 ProjPos  : SV_Position;
  float4 Color : COLOR0;
  float2 UV0 : TEXCOORD0;  //diffuse & normalmap
  $ifdef LIGHTMAP  
    float2 UV1 : TEXCOORD1;  //lightmap
  $endif  
  $ifdef FOG
    float fogCoord : TEXCOORD2;
  $endif
};

#ifdef _VISION_DX11

  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    depthFog        : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
    $ifdef SUNLIGHT
      float3    vSunDirection   : packoffset(c24);
      float3    vSunColor       : packoffset(c25);
    $endif
  }

  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4  matMV   : packoffset(c0);   // model view matrix
    float4x4  matMVP  : packoffset(c4);   // model view projection matrix
    float4x4  matMtoW : packoffset(c8);   // model to worldspace matrix
    float4    MUVtoLM : packoffset(c13);  // model UV to lightmap
  }

  cbuffer g_GlobalConstantBufferLightGrid : register (b3)
  {
    float4      Light0           : packoffset(c0);
    float4      Light1           : packoffset(c1);
    float4      Light2           : packoffset(c2);
    float4      Light3           : packoffset(c3);
    float4      Light4           : packoffset(c4);
    float4      Light5           : packoffset(c5);
  }

#else
  float4x4 matMV : register(c0);
  float4x4 matMVP : register(c8);
  float4x4 matMtoW : register(c20);
  
  float4 Light0 : register(c32);
  float4 Light1 : register(c33);
  float4 Light2 : register(c34);
  float4 Light3 : register(c35);
  float4 Light4 : register(c36);
  float4 Light5 : register(c37);

  $ifdef SUNLIGHT
    float3 vSunDirection   : register(c56);
    float3 vSunColor       : register(c57);
  $endif
  
  float4 MUVtoLM : register(c60);
  float4 depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
#endif

#ifndef USE_SKINNING
  VS_OUT vs_main( VS_IN In )
#else
  VS_OUT vs_main_skinning( VS_IN In )
#endif
{
  VS_OUT Out;
  
  float4 ObjPos;
  
  #ifndef USE_SKINNING
    ObjPos = float4(In.ObjPos, 1.0f);
  #else
    PREPARE_SKINNING(In.BoneIndices);
    TRANSFORM_OBJECT_POS(ObjPos, In.ObjPos, In.BoneWeight);
  #endif
  
  Out.ProjPos = mul( matMVP,  ObjPos);
  
  Out.UV0 = In.UV0; // base texture
  
  $ifdef LIGHTMAP  
    Out.UV1 = In.UV1 * MUVtoLM.xy + MUVtoLM.zw; // lightmap
  $endif

  $ifdef VERTEXCOLOR
    Out.Color = In.VertColor;
  $elif defined (FULLBRIGHT)
    Out.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
  $else
    Out.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
  $endif
  
  $if defined(LIGHTGRID) || defined (SUNLIGHT)
    float3 normal ;
    #ifndef USE_SKINNING
      normal = normalize(In.Normal);
    #else
      TRANSFORM_OBJECT_NORMAL(normal, In.Normal, In.BoneWeight);
    #endif
  $endif
  
  $ifdef LIGHTGRID
    Out.Color.rgb += GetLightGridDiffuse_WS( normal, Light0, Light1, Light2, Light3, Light4, Light5 );
  $endif
  
  $ifdef SUNLIGHT
    float fDotProd = saturate(-dot(normal, vSunDirection));
    Out.Color.rgb += fDotProd * vSunColor;
  $endif
  
  $ifdef FOG
    $ifdef MIRROR
      Out.fogCoord = GetFogCoord(depthFog, dot(matMV._m20_m21_m22_m23, float4(In.ObjPos, 1.0f)));
    $else
      Out.fogCoord = GetFogCoord(depthFog, Out.ProjPos.z);
    $endif
  $endif

  return Out;                              
}


