//-----------------------------------------------------------------------------------
// DynamicLightingMobileVS_Perm.inc
//-----------------------------------------------------------------------------------

#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

//-----------------------------------------------------------------------------------
// Vertex Shader Constants (non-lighting specific)

#ifdef _VISION_DX11

  $if !NOFOG || (USE_SHADOWMAP && DIRECTIONALLIGHT)
    cbuffer g_GlobalConstantBufferFrame : register(b0)
    {
      $if USE_SHADOWMAP && DIRECTIONALLIGHT
        float4 eyePos   : packoffset(c17);  // eye position
      $endif
      $if !NOFOG
        float4 depthFog : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
      $endif
    };
  $endif

  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4  matMVP  : packoffset(c4);   // model view projection matrix
    float4x4  matMtoW : packoffset(c8);   // model to worldspace matrix
  }

#else

  float4x4 matMVP        : register(c8);
  float4x4 matMtoW       : register(c20);
  $if USE_SHADOWMAP && DIRECTIONALLIGHT
    float4 eyePos        : register(c31);
  $endif
  $if !NOFOG
    float4 depthFog      : register(c62);
  $endif
  
#endif

//-----------------------------------------------------------------------------------

#include "DynamicLightingMobileHelpers.inc"

struct VS_IN
{
  float3 vObjPos  : V_VERTEX_STREAM_POSITION;  
  float3 vNormal  : V_VERTEX_STREAM_NORMAL;
  float2 UV0      : V_VERTEX_STREAM_BASETEXCOORD;
  
$ifdef MOBILE_DYNAMICLIGHTNORMALMAPPING
  float3 vTangent : V_VERTEX_STREAM_TANGENT;  
$endif
};

//-----------------------------------------------------------------------------------

VS_OUT vs_main(VS_IN In)
{
  VS_OUT Out;
  
  float4 vObjPos = float4(In.vObjPos, 1.0f); 
  Out.vProjPos = mul(matMVP, vObjPos);
  float4 vWorldPos = mul(matMtoW, vObjPos);
  Out.UV0_FogCoord = half3(In.UV0, 0.0f);
  
  // compute vector from object to light source
$if !DIRECTIONALLIGHT
  float3 vLightVec = LightPos.xyz - vWorldPos.xyz;
$endif

  half3x3 matMtoWRot = (half3x3)matMtoW;
  half3 vNormalTransformed = mul(matMtoWRot, half3(In.vNormal));
  
$if MOBILE_VERTEXLIGHTING

  // per-vertex lighting
  $if DIRECTIONALLIGHT
    Out.Color = ComputeMobileLightingDirectional(-LightDir, normalize(vNormalTransformed));
  $elif POINTLIGHT
    Out.Color = ComputeMobileLightingPoint(vLightVec, normalize(vNormalTransformed));
  $elif SPOTLIGHT
    Out.Color = ComputeMobileLightingSpot(vLightVec, -LightDir, normalize(vNormalTransformed));
  $endif
  
$else

  $if MOBILE_DYNAMICLIGHTNORMALMAPPING
  
    // compute tangent space
    vNormalTransformed = normalize(vNormalTransformed);
    half3 vTangentTransformed = normalize(mul(matMtoWRot, (half3)In.vTangent));
    half3 vBiTangentTransformed = ComputeBiNormal(vTangentTransformed, 
      vNormalTransformed, GetHandednessCorrection(In.vTangent));
    
    // transform light vector (object->light) into tangent space
    $if !DIRECTIONALLIGHT
      Out.vLightVec = GetVectorInTextureSpace(vLightVec, vTangentTransformed, 
        vNormalTransformed, vBiTangentTransformed);
    $endif
      
    // transform light direction into tangent space
    $if DIRECTIONALLIGHT || SPOTLIGHT
      Out.vLightDirTS = GetVectorInTextureSpace(-LightDir, vTangentTransformed, 
        vNormalTransformed, vBiTangentTransformed);
    $endif
    
  $else
  
    // set world space normal
    Out.vNormalWS = vNormalTransformed;
    
    $if !DIRECTIONALLIGHT
      // set light vec for point and spot lights (world space)
      Out.vLightVec = vLightVec;
    $endif
    
  $endif
  
$endif

$if USE_SHADOWMAP
  Out.ShadowCoords = mul(LightProjectionTex[0], vWorldPos);
  #if defined(_VISION_GLES2) || defined(_VISION_PSP2)
     // convert depth to OpenGL clip space in vertex shader instead later on in pixel shader 
     Out.ShadowCoords.z = 0.5f*(Out.ShadowCoords.z+Out.ShadowCoords.w);
  #endif
  $if DIRECTIONALLIGHT
    Out.ViewDistance = length(vWorldPos.xyz-eyePos.xyz);
  $endif
$endif 

  // Fog
  $if !NOFOG
    Out.UV0_FogCoord.z = GetFogCoord(depthFog, Out.vProjPos.z);
  $endif
  
  return Out;
}

//-----------------------------------------------------------------------------------
