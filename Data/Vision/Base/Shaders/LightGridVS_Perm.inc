#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include "SkinningVS.inc"

#ifdef _VISION_DX11

  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    eyePos          : packoffset(c17);  // eye position
    float4    depthFog        : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
  }

  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4  matMV   : packoffset(c0);   // model view matrix
    float4x4  matMVP  : packoffset(c4);   // model view projection matrix
    float4x4  matMtoW : packoffset(c8);   // model to worldspace matrix
    float4    MUVtoLM : packoffset(c13);  // model UV to lightmap
  }
  
  $if LIGHTGRIDSIMPLE
    cbuffer g_GlobalConstantBufferLightGrid : register (b3)
    {
      float4 StaticLightDir : packoffset(c2);
    }  
  $endif   
  
#else

  float4x4 matMV : register(c0);
  float4x4 matMVP : register(c8);
  float4x4 matMtoW : register(c20);
  float3 eyePos : register(c31);
  float4 depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  
  $ifdef LIGHTGRIDSIMPLE
    float4 StaticLightDir : register(c34);
  $endif

#endif

struct VS_IN                   
{                              
  float3 ObjPos   : V_VERTEX_STREAM_POSITION;

  float3 Normal : V_VERTEX_STREAM_NORMAL;

  float2 UV0 : V_VERTEX_STREAM_BASETEXCOORD;
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent : V_VERTEX_STREAM_TANGENT;
  $endif

  SKINNING_VERTEX_STREAMS
};       

struct VS_OUT                                 
{                                             
  float4 ProjPos  : SV_Position;              
  float2 UV0 : TEXCOORD0;

  float3 Normal  : TEXCOORD1;
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent : TEXCOORD2;
    float3 BiTangent: TEXCOORD3;
  $endif
  
  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 EyeDir : TEXCOORD4;
  $endif
  
  $if defined(USE_FOG) || defined(LIGHTGRIDSIMPLE)
    float4 LightDirFogCoord : TEXCOORD5;
  $endif
};

#ifndef USE_SKINNING
  VS_OUT vs_main( VS_IN In )
#else
  VS_OUT vs_main_skinning( VS_IN In )
#endif
{
  VS_OUT Out = (VS_OUT)0.0f;

  #ifndef USE_SKINNING
    float3 objectSpaceNormal = In.Normal;
    float4 objectSpacePos = float4(In.ObjPos, 1.0f);
  #else
    float4 objectSpacePos;
    float3 objectSpaceNormal;
    PREPARE_SKINNING(In.BoneIndices);
    TRANSFORM_OBJECT_POS(objectSpacePos, In.ObjPos, In.BoneWeight);
    TRANSFORM_OBJECT_NORMAL(objectSpaceNormal, In.Normal, In.BoneWeight);
  #endif
  
  $ifdef VMATERIAL_NORMALMAP
    #ifndef USE_SKINNING
      float3 objectSpaceTangent = In.Tangent;
    #else
      float3 objectSpaceTangent;
      TRANSFORM_OBJECT_TANGENT(objectSpaceTangent, In.Tangent, In.BoneWeight);
    #endif
  $endif

  Out.ProjPos = mul( matMVP, objectSpacePos );  
  
  Out.UV0.xy = In.UV0;
  
  //output normal, tangent and bi-tangent in worldspace
  Out.Normal = normalize(mul((float3x3)matMtoW , objectSpaceNormal));
  
  $ifdef VMATERIAL_NORMALMAP
	Out.Tangent = normalize(mul((float3x3)matMtoW , objectSpaceTangent));
    Out.BiTangent = ComputeBiNormal(Out.Tangent, Out.Normal, GetHandednessCorrection(In.Tangent));
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    Out.EyeDir = eyePos-mul(matMtoW, objectSpacePos).xyz;
  $endif
  
  $ifdef USE_FOG
    Out.LightDirFogCoord.w = GetFogCoord(depthFog, Out.ProjPos.z);
  $endif
  
  $if defined(VMATERIAL_NORMALMAP) && defined(LIGHTGRIDSIMPLE)
    Out.LightDirFogCoord.x = dot(Out.Tangent, StaticLightDir.xyz);
    Out.LightDirFogCoord.y = dot(Out.BiTangent, StaticLightDir.xyz);
    Out.LightDirFogCoord.z = dot(Out.Normal, StaticLightDir.xyz);
  $elif LIGHTGRIDSIMPLE
    Out.LightDirFogCoord.xyz = StaticLightDir.xyz;
  $endif

  return Out;                              
}



