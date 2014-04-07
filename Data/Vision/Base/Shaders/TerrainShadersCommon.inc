
#ifdef _VISION_DX11

#ifdef _IS_VERTEX_SHADER
cbuffer g_CustomConstantBufferTerrain_VS : register (b3)
{
  float4   SectorScaleOfs             : packoffset(c0); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
  float4   UVScaleOfs                 : packoffset(c1); // maps incoming [0..1] sector UV range to global [0..1] range; xy: scaling, zw:ofs
  
  // maps incoming [0..1] mesh UV range to sector [0..1] range; xy: scaling, zw:ofs
  // Note that the mapping doesn't fill the entire [0..1] range, instead for each draw call the UVs are slightly offset inwards
  // so that per-sector textures will not be sampled across page boundaries. This is important for e.g. the weightmap, as the meaning
  // of each weightmap component changes across pages.
  float4   UVSectorScaleOfs           : packoffset(c2); 
}
#endif //_IS_VERTEX_SHADER



#ifdef _IS_PIXEL_SHADER
cbuffer g_CustomConstantBufferTerrain_PS : register (b3)
{
  // up to 16 tiling factors (don't export because the full block is set by the rendering code)
  float4   _NoExport_DetailTilingBase : packoffset(c0); // this index matches the sampler index for DetailBase
  float4   _NoExport_DetailTiling0    : packoffset(c1); // DetailSampler0
  float4   _NoExport_DetailTiling1    : packoffset(c2); // DetailSampler1
  float4   _NoExport_DetailTiling2    : packoffset(c3); // DetailSampler2
  float4   _NoExport_DetailTiling3    : packoffset(c4); // DetailSampler3
  float4 _NoExport_NormalmapTransform : packoffset(c5); // register to store scale/bias value to map the normalmap UV (remapping for the overlapping at sector borders)

#ifdef VMATERIAL_USE2WEIGHTMAPS
  float4   _NoExport_DetailTiling4    : packoffset(c7); // matches sampler mapping
  float4   _NoExport_DetailTiling5    : packoffset(c8);
  float4   _NoExport_DetailTiling6    : packoffset(c9);
  float4   _NoExport_DetailTiling7    : packoffset(c10);
#endif

#ifdef USE_3WAY
  float4 DetailRemapping0 : packoffset(c7);
  float4 DetailRemapping1 : packoffset(c8);
  float4 DetailRemapping2 : packoffset(c9);
  float4 DetailRemapping3 : packoffset(c10);
#endif
}
#endif

  // samplers
  Texture2D Weightmap0              : register(t0);
  sampler   Weightmap0Sampler       : register(s0);
  Texture2D Normalmap               : register(t1);
  sampler   NormalmapSampler        : register(s1);
  Texture2D DetailBase              : register(t2);
  sampler   DetailBaseSampler       : register(s2);
  Texture2D DetailTexture0          : register(t3);
  sampler   DetailTexture0Sampler   : register(s3);
  Texture2D DetailTexture1          : register(t4);
  sampler   DetailTexture1Sampler   : register(s4);
  Texture2D DetailTexture2          : register(t5);
  sampler   DetailTexture2Sampler   : register(s5);
  Texture2D DetailTexture3          : register(t6);
  sampler   DetailTexture3Sampler   : register(s6);
  
  #ifdef VMATERIAL_NORMALMAP
    Texture2D NormalmapBase           : register(t8);
    sampler   NormalmapBaseSampler    : register(s8);
    Texture2D DetailNormalmap0        : register(t9);
    sampler   DetailNormalmap0Sampler : register(s9);
    Texture2D DetailNormalmap1        : register(t10);
    sampler   DetailNormalmap1Sampler : register(s10);
    Texture2D DetailNormalmap2        : register(t11);
    sampler   DetailNormalmap2Sampler : register(s11);
    Texture2D DetailNormalmap3        : register(t12);
    sampler   DetailNormalmap3Sampler : register(s12);
  #endif

  #ifdef VMATERIAL_USE2WEIGHTMAPS
    Texture2D Weightmap1              : register(t8);
    sampler   Weightmap1Sampler       : register(s8);
    Texture2D DetailTexture4          : register(t9);
    sampler   DetailTexture4Sampler   : register(s9);
    Texture2D DetailTexture5          : register(t10);
    sampler   DetailTexture5Sampler   : register(s10);
    Texture2D DetailTexture6          : register(t11);
    sampler   DetailTexture6Sampler   : register(s11);
    Texture2D DetailTexture7          : register(t12);
    sampler   DetailTexture7Sampler   : register(s12);
  #endif
  
   // additional samplers
  #ifdef VMATERIAL_USELIGHTMAPS
    Texture2D Lightmap                : register(t7);
    sampler   LightmapSampler         : register(s7);
  #endif
  
  #ifdef VMATERIAL_DOT3LIGHTMAPS
    Texture2D Lightmap1               : register(t13);
    sampler   Lightmap1Sampler        : register(s13);
    Texture2D Lightmap2               : register(t14);
    sampler   Lightmap2Sampler        : register(s14);
    Texture2D Lightmap3               : register(t15);
    sampler   Lightmap3Sampler        : register(s15);
  #endif

#else // _VISION_DX11

  float4 SectorScaleOfs : register(c38); // maps incoming sector xy [0..1] to world space xy position; xy: scaling, zw:ofs
  float4 UVScaleOfs : register(c39); // maps incoming [0..1] sector UV range to global [0..1] range; xy: scaling, zw:ofs
  
  // maps incoming [0..1] mesh UV range to sector [0..1] range; xy: scaling, zw:ofs
  // Note that the mapping doesn't fill the entire [0..1] range, instead for each draw call the UVs are slightly offset inwards
  // so that per-sector textures will not be sampled across page boundaries. This is important for e.g. the weightmap, as the meaning
  // of each weightmap component changes across pages.
  float4 UVSectorScaleOfs  : register(c40);

  // up to 16 tiling factors (don't export because the full block is set by the rendering code)
  float4 _NoExport_DetailTilingBase: register(c2); // this index matches the sampler index for DetailBase
  float4 _NoExport_DetailTiling0 : register(c3); // DetailSampler0
  float4 _NoExport_DetailTiling1 : register(c4); // DetailSampler1
  float4 _NoExport_DetailTiling2 : register(c5); // DetailSampler2
  float4 _NoExport_DetailTiling3 : register(c6); // DetailSampler3
  float4 _NoExport_NormalmapTransform : register(c7); // register to store scale/bias value to map the normalmap UV (remapping for the overlapping at sector borders)
  
#ifdef VMATERIAL_USE2WEIGHTMAPS
  float4 _NoExport_DetailTiling4 : register(c9); // matches sampler mapping
  float4 _NoExport_DetailTiling5 : register(c10);
  float4 _NoExport_DetailTiling6 : register(c11);
  float4 _NoExport_DetailTiling7 : register(c12);
#endif

#ifdef USE_3WAY
  float4 DetailRemapping0 : register(c9);
  float4 DetailRemapping1 : register(c10);
  float4 DetailRemapping2 : register(c11);
  float4 DetailRemapping3 : register(c12);
#endif
  
  // samplers
  sampler2D Weightmap0 : register(s0);
  sampler2D Normalmap : register(s1);
  sampler2D DetailBase : register(s2);
  sampler2D DetailTexture0 : register(s3);
  sampler2D DetailTexture1 : register(s4);
  sampler2D DetailTexture2 : register(s5);
  sampler2D DetailTexture3 : register(s6);

  #ifdef VMATERIAL_NORMALMAP
   sampler2D NormalmapBase : register(s8);
   sampler2D DetailNormalmap0 : register(s9);
   sampler2D DetailNormalmap1 : register(s10);
   sampler2D DetailNormalmap2 : register(s11);
   sampler2D DetailNormalmap3 : register(s12);
  #endif
  
  #ifdef VMATERIAL_USE2WEIGHTMAPS
   sampler2D Weightmap1        : register(s8);
   sampler2D DetailTexture4    : register(s9);
   sampler2D DetailTexture5    : register(s10);
   sampler2D DetailTexture6    : register(s11);
   sampler2D DetailTexture7    : register(s12);
  #endif
 
  #ifdef VMATERIAL_USELIGHTMAPS
    sampler2D Lightmap : register(s7);
  #endif
  
  #ifdef VMATERIAL_DOT3LIGHTMAPS
    sampler2D Lightmap1 : register(s13);
    sampler2D Lightmap2 : register(s14);
    sampler2D Lightmap3 : register(s15);
  #endif

#endif

// Terrain blending functions
#ifdef _IS_PIXEL_SHADER

float3 GetBlendWeights(float3 worldNormal)
{
  float3 blendWeights = abs(worldNormal);   
  blendWeights = max((blendWeights - 0.4), 0);  // Tighten up the blending zone
  blendWeights /= blendWeights.x + blendWeights.y + blendWeights.z;
  return blendWeights;
}

float4 GetCoordsCorrection(float3 worldNormal)
{
  float2 s = sign(worldNormal.xy);
  return float4(s.x, -s.y, s.y, 1.0);
}

#ifdef USE_3WAY
float4 MixColors(float4 colB, float4 col0, float4 col1, float4 col2, float4 col3, float4 weight);
#else
float4 MixColors(float4 colB, float4 col0, float4 col1, float4 col2, float4 col3, float4 weight)
{
  float4 result = lerp(colB,col0,weight.r*col0.a);
  result = lerp(result,col1,weight.g*col1.a);
  result = lerp(result,col2,weight.b*col2.a);
  result = lerp(result,col3,weight.a*col3.a);
  return result;
}
#endif

#ifdef _VISION_DX11
float4 SampleLayer(Texture2D tex, sampler texSampler, float3 UVW, float4 tiling, float3 blendWeights, bool use3Way, float4 cc)
#else
float4 _SampleLayer(sampler2D tex, float3 UVW, float4 tiling, float3 blendWeights, bool use3Way, float4 cc)
#endif
{
  if (use3Way)
  {
    float2 coord1 = UVW.yz * cc.xw;
    float2 coord2 = UVW.xz * cc.yw;
    float2 coord3 = UVW.xy;

    float4 col1 = vTex2D(tex, texSampler, coord1 * tiling.xy + tiling.zw) * blendWeights.x; //* float4(1.0, 0.5, 0.5, 1.0);
    float4 col2 = vTex2D(tex, texSampler, coord2 * tiling.xy + tiling.zw) * blendWeights.y; //* float4(0.5, 1.0, 0.5, 1.0);
    float4 col3 = vTex2D(tex, texSampler, coord3 * tiling.xy + tiling.zw) * blendWeights.z; //* float4(0.5, 0.5, 1.0, 1.0);
    return (col1 + col2 + col3);
  }
  else
  {
    return vTex2D(tex, texSampler, UVW.xy * tiling.xy + tiling.zw);
  }
}

#ifndef _VISION_DX11
#define SampleLayer(texture, texSampler, UVW, tiling, blendWeights, use3Way, cc) _SampleLayer(texture, UVW, tiling, blendWeights, use3Way, cc)
#endif

#ifdef _VISION_DX11
float4 SampleBumpLayer(Texture2D tex, sampler texSampler, float3 UVW, float4 tiling, float3 blendWeights, bool use3Way, float4 cc)
#else
float4 _SampleBumpLayer(sampler2D tex, float3 UVW, float4 tiling, float3 blendWeights, bool use3Way, float4 cc)
#endif
{
  if (use3Way)
  {
    float2 coord1 = UVW.yz * cc.xw;
    float2 coord2 = UVW.xz * cc.yw;
    float2 coord3 = UVW.xy;

    float4 col1 = vTex2D(tex, texSampler, coord1 * tiling.xy + tiling.zw);
    float4 col2 = vTex2D(tex, texSampler, coord2 * tiling.xy + tiling.zw);
    float4 col3 = vTex2D(tex, texSampler, coord3 * tiling.xy + tiling.zw);
    
    col1.xyz = (col1.yxz - 0.5) * cc.xxw * blendWeights.x; // swizzle to compensate for different projection spaces        
    col2.xyz = (col2.xyz - 0.5) * cc.yzw * blendWeights.y;
    col3.xyz = (col3.xyz - 0.5) * blendWeights.z;
    
    float4 result = col1 + col2 + col3;
    result.xyz = result.xyz + 0.5;
    return result;
  }
  else
  {
    return vTex2D(tex, texSampler, UVW.xy * tiling.xy + tiling.zw);
  }
}

#ifndef _VISION_DX11
#define SampleBumpLayer(texture, texSampler, UVW, tiling, blendWeights, use3Way, cc) _SampleBumpLayer(texture, UVW, tiling, blendWeights, use3Way, cc)
#endif

struct TerrainLayerConfig
{
  float2 weightUV;
  float3 UVW;
  float4 layerBlendWeights;
  float3 threeWayBlendWeights;
  float4 threeWayCoordsCorrection;
  int numThreeWayLayer;
};

TerrainLayerConfig CreateTerrainLayerConfig(float2 weightUV, float3 UVW, float3 worldNormal)
{
  TerrainLayerConfig config;
  config.weightUV = weightUV;
  config.UVW = UVW;
  config.layerBlendWeights = vTex2D(Weightmap0, Weightmap0Sampler, weightUV);
  config.threeWayBlendWeights = GetBlendWeights(worldNormal);
  config.threeWayCoordsCorrection = GetCoordsCorrection(worldNormal);
  config.numThreeWayLayer = 0;
  return config;
}

TerrainLayerConfig CreateTerrainLayerConfig3Way(float2 weightUV, float3 UVW, float3 worldNormal);

float4 GetTerrainDiffuseColor(TerrainLayerConfig config)
{
  float3 UVW = config.UVW;
  float3 blendWeights = config.threeWayBlendWeights;
  float4 cc = config.threeWayCoordsCorrection;

  float4 colB = SampleLayer(DetailBase, DetailBaseSampler, UVW, _NoExport_DetailTilingBase, blendWeights, false, cc);
  float4 col0 = SampleLayer(DetailTexture0, DetailTexture0Sampler, UVW, _NoExport_DetailTiling0, blendWeights, config.numThreeWayLayer >= 1, cc);
  float4 col1 = SampleLayer(DetailTexture1, DetailTexture1Sampler, UVW, _NoExport_DetailTiling1, blendWeights, config.numThreeWayLayer >= 2, cc);
  float4 col2 = SampleLayer(DetailTexture2, DetailTexture2Sampler, UVW, _NoExport_DetailTiling2, blendWeights, config.numThreeWayLayer >= 3, cc);
  float4 col3 = SampleLayer(DetailTexture3, DetailTexture3Sampler, UVW, _NoExport_DetailTiling3, blendWeights, config.numThreeWayLayer >= 4, cc);
  float4 diffuse = MixColors(colB, col0, col1, col2, col3, config.layerBlendWeights);
  
  #ifdef VMATERIAL_USE2WEIGHTMAPS
    float4 wgt2 = vTex2D(Weightmap1, Weightmap1Sampler, config.weightUV);
    float4 col4 = vTex2D(DetailTexture4, DetailTexture4Sampler, UVW.xy*_NoExport_DetailTiling4.xy + _NoExport_DetailTiling4.zw);
    float4 col5 = vTex2D(DetailTexture5, DetailTexture5Sampler, UVW.xy*_NoExport_DetailTiling5.xy + _NoExport_DetailTiling5.zw);
    float4 col6 = vTex2D(DetailTexture6, DetailTexture6Sampler, UVW.xy*_NoExport_DetailTiling6.xy + _NoExport_DetailTiling6.zw);
    float4 col7 = vTex2D(DetailTexture7, DetailTexture7Sampler, UVW.xy*_NoExport_DetailTiling7.xy + _NoExport_DetailTiling7.zw);
    diffuse = MixColors(diffuse, col4, col5, col6, col7, wgt2);
  #endif

  return diffuse;
}

#ifdef VMATERIAL_NORMALMAP

float4 GetTerrainDetailNormal(TerrainLayerConfig config)
{
  float3 UVW = config.UVW;
  float3 blendWeights = config.threeWayBlendWeights;
  float4 cc = config.threeWayCoordsCorrection;

  float4 nrmlB = SampleBumpLayer(NormalmapBase, NormalmapBaseSampler, UVW, _NoExport_DetailTilingBase, blendWeights, false, cc);
  float4 nrml0 = SampleBumpLayer(DetailNormalmap0, DetailNormalmap0Sampler, UVW, _NoExport_DetailTiling0, blendWeights, config.numThreeWayLayer >= 1, cc);
  float4 nrml1 = SampleBumpLayer(DetailNormalmap1, DetailNormalmap1Sampler, UVW, _NoExport_DetailTiling1, blendWeights, config.numThreeWayLayer >= 2, cc);
  float4 nrml2 = SampleBumpLayer(DetailNormalmap2, DetailNormalmap2Sampler, UVW, _NoExport_DetailTiling2, blendWeights, config.numThreeWayLayer >= 3, cc);
  float4 nrml3 = SampleBumpLayer(DetailNormalmap3, DetailNormalmap3Sampler, UVW, _NoExport_DetailTiling3, blendWeights, config.numThreeWayLayer >= 4, cc);  
  return MixColors(nrmlB, nrml0, nrml1, nrml2, nrml3, config.layerBlendWeights);
}

#endif

#endif // _IS_PIXEL_SHADER

#ifdef _IS_VERTEX_SHADER

struct VS_IN
{
  float2 ObjPosXY : POSITION; // shared stream
  float ObjPosZ : TEXCOORD0;  // sector's own stream
};

// calculates the world space vertex position
float4 GetVertexPosition(VS_IN In)
{
  float4 pos = float4(In.ObjPosXY, In.ObjPosZ, 1.0);
  pos.xy = pos.xy*SectorScaleOfs.xy + SectorScaleOfs.zw;
  return pos;
}

// maps the local UV to global [0..1] UV, but clamped so that filtering will not read samples from neighboring pages
float4 GetUV(float2 UV)
{
  float4 result;
  result.xy = UV*UVScaleOfs.xy+UVScaleOfs.zw;
  result.zw = UV*UVSectorScaleOfs.xy+UVSectorScaleOfs.zw;
  return result;
}

float4 GetWorldUV(VS_IN In)
{
  float4 pos = float4(In.ObjPosXY, In.ObjPosZ, 1.0);
  pos.xy = pos.xy * UVScaleOfs.xy + UVScaleOfs.zw;
  pos.z = (-pos.z / SectorScaleOfs.x) * UVScaleOfs.x;
  return pos;
}

#endif // _IS_VERTEX_SHADER 
