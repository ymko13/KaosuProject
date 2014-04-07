//-----------------------------------------------------------------------------------
// DynamicLightingMobilePS_Perm.inc
//-----------------------------------------------------------------------------------

#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

//-----------------------------------------------------------------------------------
// Textures

#ifdef _VISION_DX11

  Texture2D   BaseTexture         : register(t0);
  sampler     BaseTextureSampler  : register(s0);
  
  $if MOBILE_DYNAMICLIGHTNORMALMAPPING
    Texture2D   NormalMap         : register(t2);
    sampler     NormalMapSampler  : register(s2);
  $endif
  
  $ifdef USE_SHADOWMAP
    Texture2D   ShadowTex           : register(t3);
    sampler     ShadowTexSampler    : register(s3);
  $endif

#else

  sampler2D   BaseTexture         : register(s0);
  
  $if MOBILE_DYNAMICLIGHTNORMALMAPPING
    sampler2D  NormalMap          : register(s2);
  $endif
  
  $ifdef USE_SHADOWMAP
    sampler2D  ShadowTex         : register(s3);
  $endif

#endif

//-----------------------------------------------------------------------------------

$if MOBILE_DYNAMICLIGHTNORMALMAPPING
  // needs to be set for GetTangentSpaceNormal() in vMaterialHelpers.inc
  #define VMATERIAL_NORMALMAP 
$endif

#include "DynamicLightingMobileHelpers.inc"
#include "vMaterialHelpers.inc"

$if USE_SHADOWMAP
  #include "ShadowMapMobileHelpers.inc" 
$endif

//-----------------------------------------------------------------------------------

half4 ps_main(VS_OUT In) : SV_Target
{
$if MOBILE_VERTEXLIGHTING

  // get interpolated color
  half4 DynamicLightColor = In.Color;
  
$elif MOBILE_DYNAMICLIGHTNORMALMAPPING

  // sample normal map
  half4 NormalMapValue = GetTangentSpaceNormal(In.UV0_FogCoord.xy);

  // light vector (light dir) is in tangent space -> no need to transform the sampled normal  
  $if DIRECTIONALLIGHT
    half4 DynamicLightColor = ComputeMobileLightingDirectional(normalize(In.vLightDirTS), 
      NormalMapValue.xyz);
  $elif POINTLIGHT
    half4 DynamicLightColor = ComputeMobileLightingPoint(In.vLightVec, NormalMapValue.xyz);
  $elif SPOTLIGHT
    half4 DynamicLightColor = ComputeMobileLightingSpot(In.vLightVec, normalize(In.vLightDirTS), 
      NormalMapValue.xyz);
  $endif
  
$else

  $if DIRECTIONALLIGHT
    half4 DynamicLightColor = ComputeMobileLightingDirectional(-LightDir, normalize(In.vNormalWS));
  $elif POINTLIGHT
    half4 DynamicLightColor = ComputeMobileLightingPoint(In.vLightVec, normalize(In.vNormalWS));
  $elif SPOTLIGHT
    half4 DynamicLightColor = ComputeMobileLightingSpot(In.vLightVec, -LightDir, normalize(In.vNormalWS));
  $endif
    
$endif

$if USE_SHADOWMAP 
  half shadowTerm = ComputeShadowTerm(In.ShadowCoords);
  $if DIRECTIONALLIGHT
    shadowTerm = FadeoutShadow(shadowTerm, In.ViewDistance, ShadowParameters.xy);
  $endif
  DynamicLightColor.rgb *= shadowTerm;
$endif

  // get basemap color
  half4 BaseMapValue = GetDiffuseTexture(In.UV0_FogCoord.xy);
  
  half4 ResColor = BaseMapValue * half4(DynamicLightColor.rgb, 1.0f);

  $if !NOFOG
    // use black as fog color (dynamic lighting is additive)
    ResColor.rgb *= half(1.0f) - saturate(In.UV0_FogCoord.z);
  $endif
  
  return ResColor;
}

//-----------------------------------------------------------------------------------
