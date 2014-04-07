#include <shaders/StaticLightingMobileHelpers.inc>

#ifdef _VISION_DX11

  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    Timer           : packoffset(c16);
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

  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    $if VMATERIAL_BUMP && !LIGHTGRIDSIMPLE
      float4 StaticLightDir : packoffset(c0);
    $endif  
    float4 ScrollSpeed : packoffset(c1);   

    $if MOBILE_DYNAMICBASEPASSLIGHTING || (USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP)
	  float4 LightPos : packoffset(c2);

	  $if (MOBILE_VERTEXLIGHTING && (DIRECTIONALLIGHT || SPOTLIGHT)) || (USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP && DIRECTIONALLIGHT)
	    float4 LightDir : packoffset(c5);
      $endif
	$endif

    $if MOBILE_DYNAMICBASEPASSLIGHTING && MOBILE_VERTEXLIGHTING
      float4 LightRadius : packoffset(c3);
      float4 LightColor : packoffset(c4);    
      $if SPOTLIGHT
        float4 LightAngle : packoffset(c6);
      $endif
    $endif

    $if USE_SHADOWMAP
      float4x4 LightProjectionTex[4] : packoffset(c7);
    $endif
  } 

  $if LIGHTGRIDSIMPLE
    cbuffer g_GlobalConstantBufferLightGrid : register (b3)
    {
      float4 StaticLightDir : packoffset(c2);
    }  
  $endif    
#else
  float4x4 matMVP  : register(c8);
  float4x4 matMtoW : register(c20);
  float4 eyePos    : register(c31);

  $ifdef LIGHTGRIDSIMPLE
    float4 StaticLightDir : register(c34);
  $elif VMATERIAL_BUMP
    float4 StaticLightDir : register(c64);
  $endif

  $if LIGHTMAP
    float4 MUVtoLM : register(c60);
  $endif
  
  $if !NOFOG
    float4 depthFog : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  $endif

  $if VMATERIAL_BUMPANIMATED
    float4 Timer : register(c30);
    float4 ScrollSpeed : register(c65);
  $endif

	$if MOBILE_DYNAMICBASEPASSLIGHTING || (USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP)
	  float4 LightPos : register(c66); 

	  $if (MOBILE_VERTEXLIGHTING && (DIRECTIONALLIGHT || SPOTLIGHT)) || (USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP && DIRECTIONALLIGHT)
	    float4 LightDir : register(c69);
      $endif
	$endif

  $if MOBILE_DYNAMICBASEPASSLIGHTING && MOBILE_VERTEXLIGHTING
    float4 LightRadius : register(c67);
    float4 LightColor : register(c68); 
    $if SPOTLIGHT
      float4 LightAngle : register(c70);
    $endif
  $endif
  
  $if USE_SHADOWMAP
    float4x4 LightProjectionTex[4] : register(c72);
  $endif  
#endif

$if MOBILE_DYNAMICBASEPASSLIGHTING && MOBILE_VERTEXLIGHTING
  #include "DynamicLightingEquationsMobile.inc"
$endif

struct VS_IN
{
  float3 VertexPosMS : V_VERTEX_STREAM_POSITION;

  // note that we need to use all this permutation variables here, otherwise the stream mask will not be correctly set 
  $if VMATERIAL_BUMP || VMATERIAL_BUMPANIMATED || VMATERIAL_CUBEMAP || VMATERIAL_BUMPCUBEMAP || LIGHTGRIDSIMPLE || MOBILE_DYNAMICBASEPASSLIGHTING || VMATERIAL_FAKESPECULAR || (USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP)
    float3 NormalMS : V_VERTEX_STREAM_NORMAL;
  $endif
  
  float2 UV0 : V_VERTEX_STREAM_BASETEXCOORD;
  
  $if LIGHTMAP
    float2 UV1 : V_VERTEX_STREAM_LIGHTMAPCOORD;
  $endif
  
  $if VMATERIAL_BUMP || VMATERIAL_BUMPANIMATED || VMATERIAL_BUMPCUBEMAP || MOBILE_DYNAMICLIGHTNORMALMAPPING || VMATERIAL_FAKESPECULAR
    float3 TangentMS : V_VERTEX_STREAM_TANGENT;
  $endif
};

PS_IN vs_main(VS_IN In)
{
  PS_IN Out;
  float4 VertexPosMS = float4(In.VertexPosMS, 1.0f);
  Out.ProjPos = mul(matMVP, VertexPosMS);  
  
  Out.UV0 = half4(In.UV0, 0.0h, 0.0h); // base texture
  $if LIGHTMAP
    Out.UV1 = half4(In.UV1 * MUVtoLM.xy + MUVtoLM.zw, 0.0h, 0.0h);
  $else
    Out.UV1 = Out.UV0;
  $endif
 
  $if defined(VMATERIAL_BUMPANIMATED)
    Out.UV1 = Out.UV1.xyxy + Timer.xxxx * ScrollSpeed;
  $endif
  
  #ifdef CALCULATE_POSITIONWS
    float4 VertexPosWS = mul(matMtoW, VertexPosMS);
  #endif
  
  #ifdef USE_EYEDIRWS
    Out.EyeDirWS = float4(VertexPosWS.xyz - eyePos.xyz, 1.0f);
    $if (USE_SHADOWMAP && DIRECTIONALLIGHT)
      Out.EyeDirWS.w = length(Out.EyeDirWS.xyz);
    $endif
  #endif
  
  #if defined(USE_NORMALS_VS)
    half3x3 matMtoWRot = (half3x3)matMtoW;
    half3 NormalWS = normalize(mul(matMtoWRot, half3(In.NormalMS)));
  #endif
  
  #if defined(USE_NORMALS_PS)
    Out.NormalWS = NormalWS;
  #endif
  
  #if defined(USE_TANGENTSPACE_VS)
    half3 TangentWS = normalize(mul(matMtoWRot, half3(In.TangentMS)));
    half fMul = sign(dot(half3(In.TangentMS), half3(In.TangentMS)) - 2.0h);  
  #endif
  
  #if defined(CALCULATE_BINORMAL_VS)
    half3 BiNormalWS = cross(TangentWS, NormalWS)*fMul;
  #endif
  
  #if defined(USE_TANGENTSPACE_PS)
    Out.TangentWS = half4(TangentWS, fMul);
  #endif
  
  #ifdef USE_LIGHTDIR_FOGCOORD
    Out.LightDirFogCoord = half4(0.0h, 0.0h, 0.0h, 0.0h);
  #endif
  
  $if VMATERIAL_BUMP
    Out.LightDirFogCoord.x = dot(TangentWS, StaticLightDir.xyz);
    Out.LightDirFogCoord.y = -dot(BiNormalWS, StaticLightDir.xyz);
    Out.LightDirFogCoord.z = dot(NormalWS, StaticLightDir.xyz);
  $elif LIGHTGRIDSIMPLE
    Out.LightDirFogCoord.xyz = StaticLightDir.xyz;
  $endif
  
  $if !NOFOG
    Out.LightDirFogCoord.w = GetFogCoord(depthFog, Out.ProjPos.z);
  $endif
  
  $if MOBILE_DYNAMICBASEPASSLIGHTING    
    $if !DIRECTIONALLIGHT
      float3 vLightVec = LightPos.xyz - VertexPosWS.xyz;
    $endif  
  
    $if MOBILE_VERTEXLIGHTING
      $if DIRECTIONALLIGHT
        Out.VertexColor = ComputeMobileLightingDirectional(-LightDir.xyz, NormalWS);
      $elif POINTLIGHT
        Out.VertexColor = ComputeMobileLightingPoint(vLightVec, NormalWS);
      $elif SPOTLIGHT
        Out.VertexColor = ComputeMobileLightingSpot(vLightVec, -LightDir.xyz, NormalWS);
      $endif   
    $elif MOBILE_DYNAMICLIGHTNORMALMAPPING
      // transform light vector (object->light) into tangent space
      $if !DIRECTIONALLIGHT
        Out.vLightVec = GetVectorInTextureSpace(vLightVec, TangentWS, NormalWS, BiNormalWS);
      $endif
    $else
      $if !DIRECTIONALLIGHT
        Out.vLightVec = vLightVec;
      $endif  
    $endif
  $endif
  
  $if USE_SHADOWMAP
    Out.ShadowCoords = mul(LightProjectionTex[0], VertexPosWS);
    #if defined(_VISION_GLES2) || defined(_VISION_PSP2)
      // convert depth to OpenGL clip space in vertex shader instead later on in pixel shader
      Out.ShadowCoords.z = 0.5f*(Out.ShadowCoords.z+Out.ShadowCoords.w);
    #endif

	$if MOBILE_SUBTRACTIVE_SHADOWMAP
	  half3 vLightVecN;
	  $if DIRECTIONALLIGHT
	    vLightVecN = (half3)-LightDir.xyz;
	  $else
		vLightVecN = (half3)normalize(LightPos.xyz - VertexPosWS.xyz);
	  $endif
    
	  // Clip shadows on faces that are back-facing to shadow casting light.
	  half fAngle = dot(vLightVecN, NormalWS);
	  Out.ShadowCoords.w = (fAngle>0.0f)  ? Out.ShadowCoords.w : 0.0f;
	$endif
  $endif

  return Out;
}