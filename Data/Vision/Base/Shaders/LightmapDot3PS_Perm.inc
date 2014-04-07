#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <shaders/vMaterialPermutationsStaticLit.inc>

$if defined (VMATERIAL_PARALLAX) || (defined (VMATERIAL_NORMALMAP) && defined (VMATERIAL_SPECULAR))
  V_REQUIRES_SM30_SM40L93;
$endif

#ifdef _VISION_DX11
  Texture2D BaseTexture           : register(t0);
  sampler   BaseTextureSampler    : register(s0);
  Texture2D NormalMap             : register(t1);
  sampler   NormalMapSampler      : register(s1);
  
  $if defined(VMATERIAL_NORMALMAP) || defined(VMATERIAL_SPECULAR) 
    Texture2D   Lightmap1           : register(t2);
    sampler     Lightmap1Sampler    : register(s2);
    Texture2D   Lightmap2           : register(t3);
    sampler     Lightmap2Sampler    : register(s3);
    Texture2D   Lightmap3           : register(t4);
    sampler     Lightmap3Sampler    : register(s4);
  $else
    // no dot3 lightmap
    Texture2D   Lightmap            : register(t2);
    sampler     LightmapSampler     : register(s2);
  $endif
  
  $ifdef VMATERIAL_REFMAP
    Texture2D   RefMap              : register(t5);
    sampler     RefMapSampler       : register(s5);
  $endif
  
  $ifdef USE_RIMLIGHT
    Texture2D   RimLightMap         : register(t6);
    sampler     RimLightMapSampler  : register(s6);
  $endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION
    TextureCube CubeMap             : register(t7);
    sampler     CubeMapSampler      : register(s7);
  $endif
  
  // the two GLOWMAP variants are mutually exclusive
  $if defined(VMATERIAL_GLOWMAP) || defined(VMATERIAL_GLOWMAP_ADDITIVE)
    Texture2D   GlowMap             : register(t6);
    sampler     GlowMapSampler      : register(s6);
  $endif 
  
#else
  sampler2D   BaseTexture         : register(s0);
  sampler2D   NormalMap           : register(s1);
  $if defined(VMATERIAL_NORMALMAP) || defined(VMATERIAL_SPECULAR) 
    sampler2D   Lightmap1           : register(s2);
    sampler2D   Lightmap2           : register(s3);
    sampler2D   Lightmap3           : register(s4);
  $else
    sampler2D   Lightmap            : register(s2); // no dot3 lightmap
  $endif
  
  $ifdef VMATERIAL_REFMAP
    sampler2D   RefMap              : register(s5);
  $endif
  
  $ifdef USE_RIMLIGHT
    sampler2D   RimLightMap         : register(s6);
  $endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION
    samplerCUBE CubeMap             : register(s7);
  $endif
  
  // the two GLOWMAP variants are mutually exclusive
  $if defined(VMATERIAL_GLOWMAP) || defined(VMATERIAL_GLOWMAP_ADDITIVE)
    sampler2D   GlowMap             : register(s6);
  $endif
  
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_CAFE)
  float4 lightmapMul : register(c0);
  float4 depthFogCol : register(c1);  // linear depth fog color
  float3 gAmbientCol : register(c21); // global ambient
  
  float4 MaterialParams : register(c32);
  
  $ifdef ALPHATEST
  float4 AlphaThreshold  : register(c33);
  $endif
  
  $ifdef USE_RIMLIGHT
    float2 subSurfParams : register(c34);
    float4 subSurfColMod : register(c35);
  $endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION  
    float4 CubeReflectionModulate : register(c36);
  $endif
  
  $ifdef VMATERIAL_GLOWMAP_ADDITIVE
    float3		GlowColor		: register (c37);
    float		GlowIntensity	: register (c38);
  $endif  
  
#elif defined(_VISION_DX11)

  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    depthFogCol     : packoffset(c20);  // linear depth fog color
    float4    gAmbientCol     : packoffset(c21);  // global ambient color
    float4    lightmapMul     : packoffset(c22);  // lightmap multiplier (scalar) 
  }

  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    #ifdef ALPHA_DISSOLVE
      float     DissolveAlpha : c0;
    #endif

    float4 MaterialParams; // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself!
    
    $ifdef USE_RIMLIGHT
      float2 subSurfParams;
      float4 subSurfColMod;
    $endif
    
    $ifdef VMATERIAL_CUBE_REFLECTION  
      float4 CubeReflectionModulate;
    $endif
    
    $ifdef ALPHATEST
      float AlphaThreshold;
    $endif
    
    $ifdef VMATERIAL_GLOWMAP_ADDITIVE
      float3		GlowColor;
      float		GlowIntensity;
    $endif  
  }
  
#else

  float4 lightmapMul : register(c0);
  float4 depthFogCol : register(c1);  // linear depth fog color
  
  float4 MaterialParams; // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself!
  
  $ifdef ALPHATEST
    float AlphaThreshold;
  $endif
  
  $ifdef USE_RIMLIGHT
    float2 subSurfParams;
    float4 subSurfColMod;
  $endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION  
    float4 CubeReflectionModulate;
  $endif
  
  float3 gAmbientCol : register(c21); // global ambient
  
  $ifdef VMATERIAL_GLOWMAP_ADDITIVE
    float3		GlowColor;//		: register (c16);
    float		GlowIntensity;//	: register (c17);
  $endif  
  
  #ifdef ALPHA_DISSOLVE
    float DissolveAlpha;
  #endif

#endif

struct PS_IN
{
  float4   ProjPos              : SV_Position;
  float2   UV0                  : TEXCOORD0;  //diffuse & normalmap
  
  $if defined (LIGHTMAP)
    float2 UV1 : TEXCOORD1;  //lightmap
  $endif  

  $ifdef USE_FOG
    float    fogCoord             : TEXCOORD2;
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3   EyeDir               : TEXCOORD3;
  $endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION
    float3x3 InverseTextureSpace  : TEXCOORD4;
  $endif
};

#include <shaders/vMaterialHelpers.inc>

float4 ps_main( PS_IN In ) : SV_Target
{
  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 vEyeNorm = normalize(In.EyeDir);
	float2 BaseMapCoord = GetBaseMapCoords(In.UV0, vEyeNorm);
  $else
    float2 BaseMapCoord = In.UV0;
  $endif

  float4 cBaseTex = GetDiffuseTexture(BaseMapCoord);
  
  $ifdef ALPHATEST
    clip(cBaseTex.a - AlphaThreshold.r);
  $endif
  
  float4 vNormal = GetTangentSpaceNormal(BaseMapCoord);
  
  $if defined(VMATERIAL_NORMALMAP) || defined(VMATERIAL_SPECULAR) 
    float4 cLightmap1 = vTex2D(Lightmap1, Lightmap1Sampler, In.UV1);
    float4 cLightmap2 = vTex2D(Lightmap2, Lightmap2Sampler, In.UV1);
    float4 cLightmap3 = vTex2D(Lightmap3, Lightmap3Sampler, In.UV1);
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_CUBE_REFLECTION)
    float4 SpecMul = GetSpecularMultiplier(BaseMapCoord, vNormal.a);
    float fSpecExp = SpecMul.a;
  $endif

  $ifdef LIGHTMAP
    $ifdef VMATERIAL_SPECULAR  
      //do specular lighting
      LIGHT_RESULT_SPEC difspec = GetLightmapDot3LightingSpec(vNormal.xyz, cLightmap1, cLightmap2, cLightmap3, vEyeNorm, fSpecExp);  
      float3 diff = difspec.diffuse.xyz;
    $else  
      //do diffuse lighting
      $ifdef VMATERIAL_NORMALMAP
        float3 diff = GetLightmapDot3LightingDiffuse(vNormal.xyz, cLightmap1, cLightmap2, cLightmap3);
      $else
        float3 diff = vTex2D(Lightmap, LightmapSampler, In.UV1);
      $endif
    $endif
  $else
    float3 diff = 0;
  $endif
  
  float4 cResColor;
	cResColor.xyz = cBaseTex.xyz * ((diff * lightmapMul.x) + gAmbientCol.xyz);
  
  
  $if defined(VMATERIAL_GLOWMAP) || defined(VMATERIAL_GLOWMAP_ADDITIVE)
    cResColor.xyz = GetGlow(BaseMapCoord, cResColor.xyz, cBaseTex.xyz);
  $endif
  
  $ifdef VMATERIAL_SPECULAR 
    //modulate the specular lighting (either by color or float)
    cResColor += difspec.specular * SpecMul;
  $endif
  
  $ifdef USE_RIMLIGHT
	cResColor.xyz += GetRimLighting(BaseMapCoord, diff, cBaseTex.xyz, vNormal.xyz, vEyeNorm);
  $endif

  $ifdef VMATERIAL_CUBE_REFLECTION
    float3 reflectionVector = reflect(In.EyeDir, vNormal.xyz);
    reflectionVector = mul(In.InverseTextureSpace, reflectionVector);
    cResColor.xyz += GetCubeMapReflection(reflectionVector, SpecMul.xyz);
  $endif

  cResColor.a = cBaseTex.a;
  
  $ifdef USE_FOG
    cResColor.xyz = GetFogColor(cResColor.xyz, depthFogCol.xyz, In.fogCoord);
  $endif
    
  #ifdef ALPHA_DISSOLVE
    cResColor.a *= DissolveAlpha;
  #endif

  return cResColor;
}


