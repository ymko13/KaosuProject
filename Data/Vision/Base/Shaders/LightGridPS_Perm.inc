#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <shaders/vMaterialPermutationsStaticLit.inc>
#include <Shaders/SkinShadingHelper.inc>

$if defined (VMATERIAL_PARALLAX) || defined (USE_RIMLIGHT) || (defined (VMATERIAL_NORMALMAP) && defined (VMATERIAL_SPECULAR))
  V_REQUIRES_SM30_SM40L93;
$endif

#ifdef _VISION_DX11
  Texture2D   BaseTexture         : register(t0);
  sampler     BaseTextureSampler  : register(s0);
  Texture2D   NormalMap           : register(t1);
  sampler     NormalMapSampler    : register(s1);
	
	$ifdef VMATERIAL_REFMAP
		Texture2D   RefMap              : register(t2);
		sampler     RefMapSampler       : register(s2);
	$endif
  
  $ifdef VMATERIAL_CUBE_REFLECTION
    TextureCube CubeMap             : register(t3);
    sampler     CubeMapSampler      : register(s3);
  $endif
  
  $ifdef USE_RIMLIGHT
    Texture2D   RimLightMap         : register(t4);
    sampler     RimLightMapSampler  : register(s4);
  $endif
  
  // the two GLOWMAP variants are mutually exclusive
  $ifdef VMATERIAL_GLOWMAP
    Texture2D   GlowMap             : register(t4);
    sampler     GlowMapSampler      : register(s4);
  $endif
  
  $ifdef VMATERIAL_GLOWMAP_ADDITIVE
    Texture2D   GlowMap             : register(t4);
    sampler     GlowMapSampler      : register(s4);
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
	Texture2D   BeckmannLookup        : register(t5);
	sampler		BeckmannLookupSampler : register(s5);
  $endif
  
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4    depthFogCol     : packoffset(c20);  // linear depth fog color
    float4    gAmbientCol     : packoffset(c21);  // global ambient color
  }
  
  
  cbuffer g_GlobalConstantBufferLightGrid : register (b3)
  {
    $ifdef LIGHTGRID
      float4      Light0           : packoffset(c0);
      float4      Light1           : packoffset(c1);
      float4      Light2           : packoffset(c2);
      float4      Light3           : packoffset(c3);
      float4      Light4           : packoffset(c4);
      float4      Light5           : packoffset(c5);
	$endif
    $ifdef LIGHTGRIDSIMPLE
      float4 AmbientColor : packoffset(c0);
      float4 DiffuseColor : packoffset(c1);
    $endif
  }

#else
  sampler2D   BaseTexture       : register(s0);
  sampler2D   NormalMap         : register(s1);
	
	$ifdef VMATERIAL_REFMAP
		sampler2D   RefMap            : register(s2);
	$endif
  $ifdef VMATERIAL_CUBE_REFLECTION
    samplerCUBE CubeMap           : register(s3);
  $endif
  
  $ifdef USE_RIMLIGHT
    sampler2D   RimLightMap         : register(s4);
  $endif
  
  // the two GLOWMAP variants are mutually exclusive
  $ifdef VMATERIAL_GLOWMAP
    sampler2D   GlowMap           : register(s4);
  $endif
  $ifdef VMATERIAL_GLOWMAP_ADDITIVE
    sampler2D   GlowMap           : register(s4);
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
	sampler2D   BeckmannLookup    : register(s5);
  $endif
  
  float4 depthFogCol : register(c1);  // linear depth fog color

  float3 gAmbientCol : register(c21); // global ambient
  
  $ifdef LIGHTGRID
    float4 Light0 : register(c26);
    float4 Light1 : register(c27);
    float4 Light2 : register(c28);
    float4 Light3 : register(c29);
    float4 Light4 : register(c30);
    float4 Light5 : register(c31);
  $endif
  
  $ifdef LIGHTGRIDSIMPLE
    float4 AmbientColor : register(c26);
    float4 DiffuseColor : register(c27);
  $endif
  
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_CAFE)
  float4 MaterialParams : register(c32);
  $ifdef ALPHATEST
		float4 AlphaThreshold  : register(c33);
  $endif
  $ifdef VMATERIAL_CUBE_REFLECTION  
    float4 CubeReflectionModulate : register(c34);
  $endif
  
  $ifdef USE_RIMLIGHT
    float2 subSurfParams : register(c34);
    float4 subSurfColMod : register(c35);
  $endif
  
  $ifdef VMATERIAL_GLOWMAP_ADDITIVE
    float3		GlowColor		: register (c35);
    float		GlowIntensity	: register (c36);
  $endif  
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
    $ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS
      float4 SkinShadingColor     : register (c37);
	$endif
    float2 SkinShadingParams    : register (c38);
  $endif
  
#elif defined(_VISION_DX11)

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
      float3	GlowColor;
      float	GlowIntensity;
    $endif  
	
    $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
      $ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS
        float4 SkinShadingColor;
	  $endif
      float2 SkinShadingParams;
    $endif
  }

#else

  float4 MaterialParams; // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias.
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
    $ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS
      float4 SkinShadingColor;
	$endif
    float2 SkinShadingParams;
  $endif
  
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
  
  #ifdef ALPHA_DISSOLVE
    float DissolveAlpha;
  #endif

#endif


struct PS_IN
{                  
  float4 ProjPos  : SV_Position;  
  float2 UV0 : TEXCOORD0;
  float3 Normal  : TEXCOORD1;
  
  $ifdef VMATERIAL_NORMALMAP
    float3 Tangent : TEXCOORD2;
    float3 BiTangent: TEXCOORD3;
  $endif
  
  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 EyeDir : TEXCOORD4;
  $endif
  
  $if defined(USE_FOG) || defined(LIGHTGRIDSIMPLE)
    float4 LightDirFogCoord : TEXCOORD5;
  $endif
};

#include <shaders/vMaterialHelpers.inc>

float4 ps_main( PS_IN In ) : SV_Target
{
  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_CUBE_REFLECTION) || defined(USE_RIMLIGHT)
    float3 vEyeNorm = normalize(In.EyeDir); // the eye dir is in world space, so parallax mapping won't work here!!!
	float2 BaseMapCoord = GetBaseMapCoords(In.UV0, vEyeNorm);
  $else
    float2 BaseMapCoord = In.UV0;
  $endif
  
  float4 cBaseTex = GetDiffuseTexture(BaseMapCoord);
  
  $ifdef ALPHATEST
    clip(cBaseTex.a - AlphaThreshold.r);
  $endif

  float4 vNormal = GetTangentSpaceNormal(BaseMapCoord);
  $ifdef VMATERIAL_NORMALMAP
    float3 worldNormal = GetWorldNormal(vNormal, In.Tangent, In.BiTangent, In.Normal);
  $else
    float3 worldNormal = normalize(In.Normal);
  $endif

  $if defined(VMATERIAL_SPECULAR) || defined(VMATERIAL_CUBE_REFLECTION)
    float4 SpecMul = GetSpecularMultiplier(BaseMapCoord, vNormal.a);
    float fSpecExp = SpecMul.a;
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
	$ifdef VMATERIAL_SPECULAR
	  float3 specular = GetLightGridSpec_KelemenSzirmay_BRDF_WS( worldNormal, vEyeNorm,
	                                                             Light0, Light1,
																 Light2, Light3,
																 Light4, Light5,
																 SkinShadingParams.x,
																 SkinShadingParams.y,
																 BeckmannLookup,
																 BeckmannLookupSampler );
	$endif
	
	$ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS
		float3 diffNormalMap = GetLightGridDiffuse_WS( worldNormal,
													   Light0, Light1,
													   Light2, Light3,
													   Light4, Light5 ).xyz;
		float3 diffGeometry  = GetLightGridDiffuse_WS( normalize(In.Normal),
													   Light0, Light1,
													   Light2, Light3,
													   Light4, Light5 ).xyz;
		float3 diff = lerp(diffGeometry, diffNormalMap, SkinShadingColor.xyz);
	$else
		float3 diff = GetLightGridDiffuse_WS( worldNormal,
													   Light0, Light1,
													   Light2, Light3,
													   Light4, Light5 ).xyz;
	$endif
  $else
      $ifdef LIGHTGRIDSIMPLE
	    $if defined(VMATERIAL_SPECULAR)
		  float3 specular = float3(0.0f, 0.0f, 0.0f);
		$endif
		$if VMATERIAL_NORMALMAP
	      // For simple bump lighting, we transformed the light direction to tangent space in the VS, so we can use that here.
		  float dotLightgrid = max(dot(vNormal.xyz, In.LightDirFogCoord.xyz), 0.0f);
		 $else
		  // For all other techniques we either use the geometric normal, or we have to transform the normal map to worldspace anyway,
		  // so there is no saving from transforming the light direction in the vertex shader first.
		  float dotLightgrid = max(dot(worldNormal, In.LightDirFogCoord.xyz), 0.0f);
		$endif
		float3 diff = AmbientColor.xyz + DiffuseColor.xyz * dotLightgrid;
	  $else
		$if defined(VMATERIAL_SPECULAR)
	      LIGHT_RESULT_SPEC res = GetLightGridSpec_WS(vEyeNorm, worldNormal, 
													  Light0, Light1, 
													  Light2, Light3, 
													  Light4, Light5, 
													  fSpecExp);
		  float3 diff = res.diffuse.xyz; 
		  float3 specular = res.specular.xyz;		
		 $else  
		  // diffuse only
		  float3 diff = GetLightGridDiffuse_WS( worldNormal,
												Light0, Light1,
												Light2, Light3,
												Light4, Light5 );
		$endif
	  $endif
  $endif

   float4 result;
   result.xyz = (diff + gAmbientCol.xyz) * cBaseTex.xyz;  

  $if defined(VMATERIAL_GLOWMAP) || defined(VMATERIAL_GLOWMAP_ADDITIVE)
    result.xyz = GetGlow(BaseMapCoord, result.xyz, cBaseTex.xyz);
  $endif
	 
  $if defined(VMATERIAL_SPECULAR)
    result.xyz += specular*SpecMul.xyz;
  $endif
  
  $ifdef USE_RIMLIGHT
	// since the eyedir is in worldspace we also need to pass the world space normal here:
	result.xyz += GetRimLighting(BaseMapCoord, diff, cBaseTex.xyz, worldNormal, vEyeNorm);
  $endif 
  
  $ifdef VMATERIAL_CUBE_REFLECTION
    float3 reflectionVector = reflect(eyeNorm, worldNormal);
    result.xyz += GetCubeMapReflection(reflectionVector, SpecMul.xyz);
  $endif

  result.a = cBaseTex.a;

  $ifdef USE_FOG
    result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, In.LightDirFogCoord.w);  
  $endif
  
  #ifdef ALPHA_DISSOLVE
    result.a *= DissolveAlpha;
  #endif

  return result;
}

