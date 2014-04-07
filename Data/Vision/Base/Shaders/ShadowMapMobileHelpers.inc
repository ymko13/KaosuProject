//-----------------------------------------------------------------------------------
// ShadowMapMobileHelpers.inc
//-----------------------------------------------------------------------------------

// Simple shadow mapping
half ComputeShadowTerm(in float4 shadowCoords)
{
$if !DIRECTIONALLIGHT
  float texDepth = vTex2Dproj(ShadowTex, ShadowTexSampler, shadowCoords).r;
$else
  float texDepth = vTex2D(ShadowTex, ShadowTexSampler, shadowCoords.xy).r;
$endif

  float compareDepth = shadowCoords.z;
$if !DIRECTIONALLIGHT
  compareDepth /= shadowCoords.w;
$endif

$if MOBILE_SUBTRACTIVE_SHADOWMAP
  // Clip shadows on faces that are back-facing to shadow casting light. Furthermore prevent artifacts in case 
  // of subtractive shadow mapping when receiver geometry is outside of clip space.
  float outsideClipspace = abs(shadowCoords.z) > shadowCoords.w;
  if (outsideClipspace)
    return 1.0f;
  return (texDepth > compareDepth);
$else
  return (texDepth > compareDepth);
$endif
}

half FadeoutShadow(in half shadowTerm, in float distance, in float2 fadeoutParams)
{
  float fFadeOut = 1.0f - saturate((fadeoutParams.x - distance) * fadeoutParams.y);  
  return saturate(shadowTerm + half(fFadeOut)); 
}