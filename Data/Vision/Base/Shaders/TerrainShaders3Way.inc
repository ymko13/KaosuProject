
#ifndef USE_3WAY
  #error define USE_3WAY before including this file!
#endif

$if (NUM_3WAY_LAYERS > 0)
  V_REQUIRES_SM30_SM40L93;
$endif

$ifdef MSAA
  #define MSAA
$endif

float4 MixColors(float4 colB, float4 col0, float4 col1, float4 col2, float4 col3, float4 weight)
{
  float4 invWeight = 1 - weight;
  float4 nrmWeight = float4(invWeight.www, 1.0);
  nrmWeight.xy *= invWeight.z;
  nrmWeight.x *= invWeight.y;
  float baseWeight = nrmWeight.x * invWeight.x;
  nrmWeight *= weight;
  
  float4 result = colB * baseWeight;
$if (NUM_3WAY_LAYERS > 0)
  result += col0 * dot(nrmWeight, DetailRemapping0);
  result += col1 * dot(nrmWeight, DetailRemapping1);
  result += col2 * dot(nrmWeight, DetailRemapping2);
  result += col3 * dot(nrmWeight, DetailRemapping3);
$else
  result += col0 * nrmWeight.x;
  result += col1 * nrmWeight.y;
  result += col2 * nrmWeight.z;
  result += col3 * nrmWeight.w;
$endif
  return result;
}

TerrainLayerConfig CreateTerrainLayerConfig3Way(float2 weightUV, float3 UVW, float3 worldNormal)
{
  TerrainLayerConfig config = CreateTerrainLayerConfig(weightUV, UVW, worldNormal);
  config.numThreeWayLayer = $NUM_3WAY_LAYERS;
  return config;
}
