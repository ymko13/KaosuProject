//-----------------------------------------------------------------------------------
// DynamicLightingEquationsMobile.inc
//-----------------------------------------------------------------------------------

// ComputeMobileLightingDirectional
//  vLightDir: normalized light direction (pointing away from the surface)
$if DIRECTIONALLIGHT
half4 ComputeMobileLightingDirectional(in half3 vLightDir, in half3 vNormal)
{
  // diffuse term
  return max(0.0f, dot(vNormal, vLightDir)) * LightColor;
}
$endif

// ComputeMobileLightingPoint
//  vLightVec: vector from object pos to light pos (not normalized)
$if POINTLIGHT
half4 ComputeMobileLightingPoint(in float3 vLightVec, in half3 vNormal)
{
  // Do not use rsqrt here (does not work correctly on Samsung Galaxy Note)
  float fLightVecLen = length(vLightVec);
  half3 vLightVecNormalized = vLightVec / fLightVecLen;

  // attenuation
  half fLightInt = LightRadius.y * fLightVecLen;
  fLightInt = saturate(1.0f - fLightInt); // simple linear attenuation function
  
  // diffuse term
  half fDiffuse = max(0.0f, dot(vNormal, vLightVecNormalized));
  
  return fLightInt * fDiffuse * LightColor;
}
$endif

// ComputeMobileLightingSpot
//  vLightVec: vector from object pos to light pos (not normalized)
//  vLightDir: normalized light direction (pointing away from the surface)
$if SPOTLIGHT
half4 ComputeMobileLightingSpot(in float3 vLightVec, in half3 vLightDir, in half3 vNormal)
{
  float fLightVecLen = length(vLightVec);
  half3 vLightVecNormalized = vLightVec / fLightVecLen;

  // attenuation
  half fLightInt = LightRadius.y * fLightVecLen;
  half2 attenuation = saturate(half2(
    (dot(vLightDir, vLightVecNormalized) - LightAngle.x)  * LightAngle.y,
    1.0f - fLightInt)); // only one saturate call
  fLightInt = attenuation.x * attenuation.y;
  
  // diffuse term
  half fDiffuse = max(0.0f, dot(vNormal, vLightVecNormalized));
  
  return fLightInt * fDiffuse * LightColor;
}
$endif





