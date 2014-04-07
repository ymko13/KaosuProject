#include <Shaders/VisionCommon.inc>
#include "ShaderHelpers.inc"
#include "SkinningVS.inc"

$if defined (VMATERIAL_PARALLAX) || (defined (VMATERIAL_NORMALMAP) && defined (VMATERIAL_SPECULAR))
  V_REQUIRES_SM30_SM40L93;
$endif

struct VS_IN
{
  float3 ObjPos   : V_VERTEX_STREAM_POSITION;

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 Normal : V_VERTEX_STREAM_NORMAL;
  $endif
  
  float2 UV0 : V_VERTEX_STREAM_BASETEXCOORD;
  $if defined (LIGHTMAP)
    float2 UV1 : V_VERTEX_STREAM_LIGHTMAPCOORD;
  $endif
  
  $if defined (VMATERIAL_SPECULAR) || defined (VMATERIAL_CUBE_REFLECTION) || defined (VMATERIAL_PARALLAX) || defined(USE_RIMLIGHT)
    float3 Tangent : V_VERTEX_STREAM_TANGENT;
  $endif

  SKINNING_VERTEX_STREAMS
};

struct VS_OUT
{
  float4 ProjPos  : SV_Position;
  float2 UV0 : TEXCOORD0;  //diffuse & normalmap
  
  $if defined (LIGHTMAP)
    float2 UV1 : TEXCOORD1;  //lightmap
  $endif  
  
  $ifdef USE_FOG
    float fogCoord : TEXCOORD2;
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 EyeDir : TEXCOORD3;
  $endif

  $ifdef VMATERIAL_CUBE_REFLECTION
    float3x3 InverseTextureSpace : TEXCOORD4;
  $endif
};

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

#else

  float4x4 matMV : register(c0);
  float4x4 matMVP : register(c8);
  float4x4 matMtoW : register(c20);
  float4 eyePos : register(c31);
  float4 MUVtoLM : register(c60);
  float4 depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  
#endif

#ifndef USE_SKINNING
  VS_OUT vs_main( VS_IN In )
#else
  VS_OUT vs_main_skinning(VS_IN In)
#endif
{
  VS_OUT Out;
  
  float4 ObjPos;
  
  #ifndef USE_SKINNING
    ObjPos = float4(In.ObjPos, 1.0f) ;
  #else
    PREPARE_SKINNING(In.BoneIndices);
    TRANSFORM_OBJECT_POS(ObjPos, In.ObjPos, In.BoneWeight);
  #endif
  
  Out.ProjPos = mul( matMVP, ObjPos);
	
  Out.UV0 = In.UV0; // base texture
  
  $ifdef LIGHTMAP
    Out.UV1 = In.UV1 * MUVtoLM.xy + MUVtoLM.zw; // lightmap
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)

    #ifndef USE_SKINNING
      float3 Normal = In.Normal;
      float3 Tangent = In.Tangent;
    #else
      float3 Normal;
      float3 Tangent;
      TRANSFORM_OBJECT_NORMAL(Normal, In.Normal, In.BoneWeight);
      TRANSFORM_OBJECT_TANGENT(Tangent, In.Tangent, In.BoneWeight);
    #endif
  
    Normal = normalize(mul((float3x3)matMtoW, Normal));
    Tangent = normalize(mul((float3x3)matMtoW, Tangent));
    float3 biNormal = ComputeBiNormal(Tangent, Normal, GetHandednessCorrection(In.Tangent));
	
    float3 tEyeDir = eyePos.xyz - mul(matMtoW, float4(In.ObjPos,1));
    Out.EyeDir = GetVectorInTextureSpace(tEyeDir, Tangent, Normal, biNormal);
  $endif

  $ifdef USE_FOG
    Out.fogCoord = GetFogCoord(depthFog, Out.ProjPos.z);
  $endif

  $ifdef VMATERIAL_CUBE_REFLECTION
    Out.InverseTextureSpace = GetInverseTextureSpace(Tangent, Normal, biNormal);
  $endif

  return Out;                              
}



