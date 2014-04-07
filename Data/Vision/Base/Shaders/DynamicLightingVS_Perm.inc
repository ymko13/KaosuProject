#include <Shaders/VisionCommon.inc>
#include "ShaderHelpers.inc"
#include "SkinningVS.inc"

$if (defined (DYNLIGHT_SPOT_PROJECTOR) || defined (DYNLIGHT_OMNI_PROJECTOR)) && defined (VMATERIAL_NORMALMAP) && defined (VMATERIAL_PARALLAX) && defined (VMATERIAL_SPECULAR)
	V_REQUIRES_SM30;
$endif

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
  float4x4 matMV         : register(c0);
  float4x4 matMVP        : register(c8);
  float4x4 matMtoW       : register(c20);
  float4   eyePos        : register(c31);
  float4   depthFog      : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
#endif

$ifdef DYNLIGHT_SPOT_PROJECTOR
  #define DYNLIGHT_USE_REFPLANES
$endif

$ifdef DYNLIGHT_OMNI_PROJECTOR
  #define DYNLIGHT_USE_REFPLANES
$endif
  
#if defined(_VISION_DX11)
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    float4 LightPos : packoffset(c0);
    #ifdef DYNLIGHT_USE_REFPLANES
      float4 RefPlaneX : packoffset(c1);
      float4 RefPlaneY : packoffset(c2);
      float4 RefPlaneW : packoffset(c3);
    #endif
    $ifdef DYNLIGHT_DIRECTIONAL
      float4 LightDir : packoffset(c4);
    $endif
  }
  
#else
  float4 LightPos : register(c64);
  #ifdef DYNLIGHT_USE_REFPLANES
    float4 RefPlaneX : register(c66);
    float4 RefPlaneY : register(c67);
    float4 RefPlaneW : register(c68);
  #endif
  $ifdef DYNLIGHT_DIRECTIONAL
    float4 LightDir : register(c69);
  $endif
  
#endif

struct VS_IN                   
{                              
  float3 ObjPos   : V_VERTEX_STREAM_POSITION;  
  float3 Normal   : V_VERTEX_STREAM_NORMAL;
  float2 UV0      : V_VERTEX_STREAM_BASETEXCOORD;  
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent : V_VERTEX_STREAM_TANGENT;  
  $endif

  SKINNING_VERTEX_STREAMS
};                                           

struct VS_OUT                                 
{                                             
  float4 ProjPos  : SV_Position;

  $ifdef SHADOWMAP
    float4 UV0 : TEXCOORD0;     // u, v, screenX, screenY
  $else
    float2 UV0 : TEXCOORD0;
  $endif

  $ifdef USE_FOG
    float4 Normal  : TEXCOORD1; // encode fogcoord into w component because of limited amount of intepolators
  $else
    float3 Normal  : TEXCOORD1;
  $endif
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent : TEXCOORD2;
    float3 BiTangent: TEXCOORD3;
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
    float3 VWorldPos : TEXCOORD4;
  $elif defined(VMATERIAL_SPECULAR)
    float3 HalfAngleWS : TEXCOORD4;
  $endif
  
  $ifdef SHADOWMAP
    float4 LightWS : TEXCOORD5;  // Lx, Ly, Lz, screenW
  $else
    float3 LightWS : TEXCOORD5;
  $endif
  
  $ifdef DYNLIGHT_OMNI_PROJECTOR
    float3 ProjDir : TEXCOORD6;
  $endif
  
  $ifdef DYNLIGHT_SPOT_PROJECTOR
    float4 ProjTexCoord : TEXCOORD6;
  $endif

  $if defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
    float3 EyeDir : TEXCOORD7;
  $endif

};                                             

#ifndef USE_SKINNING
	VS_OUT vs_main( VS_IN In )
#else
	VS_OUT vs_main_skinning( VS_IN In )
#endif
{                                              
  VS_OUT Out;

  
  #ifndef USE_SKINNING
     float4 ObjPos = float4(In.ObjPos, 1.0f);
  #else
    float4 ObjPos;
    PREPARE_SKINNING(In.BoneIndices);
    TRANSFORM_OBJECT_POS(ObjPos, In.ObjPos, In.BoneWeight);
  #endif
  
  Out.ProjPos = mul( matMVP,  ObjPos);  
  float4 WorldPos = mul(matMtoW, ObjPos);
  Out.UV0.xy = In.UV0;
  Out.LightWS.xyz = LightPos.xyz - WorldPos.xyz;
  float3 vLightWSnorm = normalize(Out.LightWS.xyz);
  
  $ifdef SHADOWMAP
    Out.UV0.zw = Out.ProjPos.xy;
    Out.UV0.w = -Out.UV0.w;
    Out.LightWS.w = Out.ProjPos.w;
  $endif
  
  //output normal, tangent and bi-tangent in worldspace
  float3 Normal;
  
  #ifndef USE_SKINNING
    Normal = In.Normal;
  #else
    TRANSFORM_OBJECT_NORMAL(Normal, In.Normal, In.BoneWeight);
  #endif
  
  Out.Normal.xyz = normalize(mul((float3x3)matMtoW, Normal));
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent;
  
    #ifndef USE_SKINNING
      Tangent = In.Tangent;
    #else
      TRANSFORM_OBJECT_TANGENT(Tangent, In.Tangent, In.BoneWeight);
    #endif
  
    Out.Tangent = normalize(mul((float3x3)matMtoW, Tangent));
    Out.BiTangent = ComputeBiNormal(Out.Tangent, Out.Normal.xyz, GetHandednessCorrection(In.Tangent));
  $endif
  
  float3 EyeDir = eyePos.xyz - WorldPos.xyz;
  
  $if defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
    Out.VWorldPos = WorldPos.xyz;
  $elif defined(VMATERIAL_SPECULAR)
    $ifdef DYNLIGHT_DIRECTIONAL
      Out.HalfAngleWS = normalize(EyeDir) - LightDir;
    $else
      Out.HalfAngleWS = normalize(EyeDir) + vLightWSnorm;
    $endif
  $endif

  $if defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
    $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	  Out.EyeDir = normalize(eyePos.xyz - WorldPos.xyz);
    $else
      Out.EyeDir = GetVectorInTextureSpace(EyeDir, Out.Tangent, Out.Normal.xyz, Out.BiTangent); 
	$endif
  $endif
  


  $ifdef DYNLIGHT_OMNI_PROJECTOR
    // rather use existing matrix (same for spotlights):
    Out.ProjDir.x = dot(vLightWSnorm,RefPlaneX.xyz);
    Out.ProjDir.y = dot(vLightWSnorm,RefPlaneY.xyz);
    Out.ProjDir.z = dot(vLightWSnorm,RefPlaneW.xyz);
  $endif
  
  $ifdef DYNLIGHT_SPOT_PROJECTOR
    //Compute projected texture coordinate
    Out.ProjTexCoord.x = dot(RefPlaneX, WorldPos);
    Out.ProjTexCoord.y = dot(RefPlaneY, WorldPos);
    Out.ProjTexCoord.w = dot(RefPlaneW, WorldPos);

    #ifdef _VISION_PSP2
      // We need a normalized distance here as backprojection have to be rejected 
      // by using a constant epsilon in the pixel shader
      Out.ProjTexCoord.z = Out.ProjTexCoord.w / length(RefPlaneW);
    #else
      Out.ProjTexCoord.z = 0.0f;
    #endif
    
    float4 bias = Out.ProjTexCoord.w*float4(0.5f, 0.5f, 0.0f, 0.0f);
    Out.ProjTexCoord += bias;
  $endif
  
  $ifdef USE_FOG
    Out.Normal.w = GetFogCoord(depthFog, Out.ProjPos.z);
  $endif
  
  return Out;                              
}

