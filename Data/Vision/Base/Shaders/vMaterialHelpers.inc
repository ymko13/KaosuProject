
float2 GetBaseMapCoords(float2 UV, float3 vEyeDir)
{
  float2 BaseMapCoord = UV;
#ifdef VMATERIAL_PARALLAX
  float fHeight = vTex2D(NormalMap, NormalMapSampler, BaseMapCoord).a * MaterialParams.z - MaterialParams.w;
  BaseMapCoord = UV - (vEyeDir.xy * fHeight);
  #ifdef VMATERIAL_PARALLAX_REFINEMENT
    fHeight += vTex2D(NormalMap, NormalMapSampler, BaseMapCoord).a * MaterialParams.z - MaterialParams.w;
    BaseMapCoord = UV - (vEyeDir.xy * fHeight * 0.5f);    
  #endif
#endif
  return BaseMapCoord;
}

#ifndef VMATERIAL_CUSTOMDIFFUSEFUNCTION
  float4 GetDiffuseTexture(float2 UV)
  {
    return vTex2D(BaseTexture, BaseTextureSampler, UV);
  }
#endif

float4 GetTangentSpaceNormal(float2 UV)
{
#ifdef VMATERIAL_NORMALMAP
  float4 normal = DecodeNormalAndAlpha(vTex2D(NormalMap, NormalMapSampler, UV));
  normal.xyz = normalize(normal.xyz);
  return normal;
#else
  return float4(0, 0, 1, 1);
#endif
}

float4 GetSpecularMultiplier(float2 UV, float normalMapAlpha)
{
  float4 specMul = MaterialParams.xxxy;
#ifdef VMATERIAL_REFMAP
  specMul *= vTex2D(RefMap, RefMapSampler, UV); //specmul masked by specmap
#else
  specMul *= normalMapAlpha;
#endif
  return specMul;
}

float3 GetRimLighting(float2 UV, float3 diffuseLit, float3 baseTex, float3 vNormal, float3 vEyeDir)
{
#ifdef USE_RIMLIGHT
  float3 cRimLightMap = vTex2D(RimLightMap, RimLightMapSampler, UV);
  float3 sub_light = clamp(diffuseLit, 0.0, 0.5);
  float rim = pow(1.0 - saturate(dot(vNormal, vEyeDir)), subSurfParams.y);
  return baseTex * subSurfColMod * rim * sub_light * subSurfParams.x * cRimLightMap;
#else
  return 0;
#endif
}

float3 GetGlow(float2 UV, float3 diffuseLit, float3 baseTex)
{
#ifdef VMATERIAL_GLOWMAP
  float3 cGlowVal = vTex2D(GlowMap, GlowMapSampler, UV).rgb;
  return lerp(diffuseLit.xyz, baseTex.xyz, cGlowVal.xxx);
#elif defined(VMATERIAL_GLOWMAP_ADDITIVE)
  float3 cGlowVal = vTex2D(GlowMap, GlowMapSampler, UV).rgb;
  return diffuseLit + cGlowVal.rgb * GlowColor.rgb * GlowIntensity;
#else
  return diffuseLit;
#endif
}

float3 GetCubeMapReflection(float3 worldSpaceReflectionVector, float3 reflectionMultiplier)
{
#ifdef VMATERIAL_CUBE_REFLECTION
  float3 cCubeRefl = vTexCUBE(CubeMap, CubeMapSampler, worldSpaceReflectionVector).xyz * CubeReflectionModulate.xyz;
  return cCubeRefl * reflectionMultiplier;
#else
  return 0;
#endif
}
