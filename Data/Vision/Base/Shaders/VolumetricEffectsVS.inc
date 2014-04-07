//
#include <Shaders/VisionCommon.inc>
#include <shaders/ShaderHelpers.inc>

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4 CameraPos : packoffset(c17);  // camera position
    float4 DepthFog : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
  }
  
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 MatMVP : packoffset(c4); // model view projection matrix
    float4x4 MatMtoW : packoffset(c8);
  }
  
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    float4 FadeParams : packoffset(c0); // (x = FadeStart, y = FadeEnd, z = FadeFalloff, w = ShrinkFadeThreshold)
    float4 EdgeFadeFalloff: packoffset(c1); 
  }
#else
  float4x4 MatMVP : register(c8);
  float4x4 MatMtoW : register(c20);
  float4 CameraPos : register(c31);
  float4 DepthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  float4 FadeParams : register(c64); // (x =FadeStart, y = FadeEnd, z = FadeFalloff, w = ShrinkFadeThreshold)
  float EdgeFadeFalloff: register(c65); 
#endif

struct VS_IN
{
  float3 ObjPos : V_VERTEX_STREAM_POSITION;
  float3 Normal : V_VERTEX_STREAM_NORMAL;
  float4 VertColor : V_VERTEX_STREAM_COLOR;
$ifdef USE_TEXTURE
  float2 UV0 : V_VERTEX_STREAM_BASETEXCOORD;
$endif
};

struct VS_OUT
{
  float4 ProjPos : SV_Position;
  half4 Color : TEXCOORD0;
$ifdef USE_TEXTURE
  half2 UV0 : TEXCOORD1;
$endif
};

VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
 
  float4 worldPos = mul(MatMtoW, float4(In.ObjPos,1.0f));
  float3 viewVec = CameraPos.xyz-worldPos.xyz;
  float viewVecLen = length(viewVec);
  
  half fDistance = half(saturate((viewVecLen-FadeParams.y)/(FadeParams.x-FadeParams.y))); 
  half fFade = pow(fDistance,(half)FadeParams.z);
  half shrinkFadeThreshold = (half)FadeParams.w;
  half zScaleFactor = (fFade > shrinkFadeThreshold) ? 1.0h : (fFade/shrinkFadeThreshold);       
  float4 objPos = float4(In.ObjPos.xy,In.ObjPos.z*zScaleFactor,1.0f);
  
  Out.ProjPos = mul(MatMVP, objPos);
  Out.Color = (half4)In.VertColor;
  Out.Color.a *= fFade;
  
$ifdef USE_EDGEFADE
  half3 viewDir = viewVec/viewVecLen;
  half3x3 matMtoWRot = (half3x3)MatMtoW;
  half3 normal = normalize(mul(matMtoWRot, (half3)In.Normal));
  half fAngle = saturate(abs(dot(normal, viewDir)));
  Out.Color.a *= pow(fAngle,(half)EdgeFadeFalloff);
$endif

$ifdef USE_FOGFADE
  Out.Color.a *= 1.0h - (half)GetFogCoord(DepthFog, Out.ProjPos.z);
$endif

$ifdef USE_TEXTURE
  Out.UV0 = (half2)In.UV0;
$endif

  return Out;
}


