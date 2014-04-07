// Transmissivity in a non-scattering homogenous medium
float BeersLaw(float alpha, float dist)
{
  return exp(-alpha*dist);
}

// Let the height-attenuated density at a given point r(t) = o + t*d along the ray be defined as
//
//   f(r(t)) = k e^(-h r(t)_z),
//
// where k is the baseline density, and h is the height falloff coefficient.  Integrating this along the ray
// yields the final density p:
//
//   p = k * |d| * (e^{h d_z} - 1) e^{-h (o+d)_z} / (h d_z).
//
// The limit of p as h->0 is k * |d|, which corresponds to unattenuated fog.  
//
// When d_z approaches zero, p goes to k * |d| * e^{-h o_z}, so we separate the definition of p into three terms:
//
//   p = |d| * p_a * p_b,
//
// where p_b is the base term k * e^{-h o_z}, and the attenuation term p_a is defined as
//
//   p_a = (e^{h d_z} - 1) e^{-h d_z} / (h d_z)
//       = (e^{h d_z} - 1) / (e^{h d_z} h d_z)
//       = (1 - e^{-h d_z}) / h d_z.
//
float ComputeFogAttenuation(float3 worldPos, float3 viewDir, float HeightFalloff)
{
  float attV = HeightFalloff * viewDir.z;
  float result = 0;

  if (abs(attV) > 0.0000001f)
  {
    float attZ = HeightFalloff * worldPos.z;
    result = (exp(-attZ) - exp(-attZ - attV)) / attV;
  }
  else
  {
    // The attenuation function has a discontinuity at zero, so we factor out exp(-attZ) and take the limit with respect to attV:
    //   lim_{a->0} (1-e^{-a}) / a
    // = lim_{a->0} [d/da 1-e^{-a}] / [d/da a]
    // = lim_{a->0} a e^{-a}
    // = 0.
  }
  
  return result;
}

float ComputeDepthFogOpacity(float scrDepth, float4 params, float3 clipPlanes)
{
  if (scrDepth >= 1.0f)
    scrDepth = params.w;

  return (scrDepth * clipPlanes.y - params.x) * params.z;
}

float ComputeLayeredFogOpacity(float3 viewDirection, float4 params, float density, float3 clipPlanes, float3 worldPos)
{
  float  HeightFalloff    = params.x;
  float  GroundOffset     = params.y;
  float2 FogExtents       = params.zw;

  float  Opacity = 0;
  
  worldPos.z -= GroundOffset;

  // Obtain the normalized screen depth
  float NormalizedDepth = (length(viewDirection) - clipPlanes.x) * clipPlanes.z;
  
  // Rescale to the fog extents (this provides the user with a linear density control in addition to the exponential one)
  float s = max(0, NormalizedDepth * (FogExtents[1] - FogExtents[0]) - FogExtents[0]);
  
  // Compute the transmissivity
  float ViewDensity = density * ComputeFogAttenuation(worldPos, viewDirection, HeightFalloff);
  
  Opacity = 1 - BeersLaw(ViewDensity, s);

  return Opacity;
}

float4 MixFog(float4 Depth, float4 Layered)
{
  return float4(Depth.w * Depth.xyz + (1 - Depth.w) * Layered.w * Layered.xyz, (1 - Depth.w) * (1 - Layered.w));
}
