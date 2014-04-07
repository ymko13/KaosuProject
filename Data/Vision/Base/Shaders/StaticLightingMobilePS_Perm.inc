#include <shaders/StaticLightingMobileHelpers.inc>

#ifdef _VISION_DX11
  Texture2D   BaseTexture         : register(t0);
  sampler     BaseTextureSampler  : register(s0);

  #if defined(USE_NORMALMAP)
    Texture2D   NormalMap         : register(t1);
    sampler     NormalMapSampler  : register(s1);
  #endif

  $ifdef LIGHTMAP
    Texture2D   Lightmap            : register(t2);
    sampler     LightmapSampler     : register(s2);
  $endif
  
  #if defined(USE_CUBEMAP)
    TextureCube CubeMap             : register(t3);
    sampler     CubeMapSampler      : register(s3);
  #endif
  
  $ifdef USE_SHADOWMAP 
    Texture2D   ShadowTex           : register(t4);
    sampler     ShadowTexSampler    : register(s4);

	$if MOBILE_SUBTRACTIVE_SHADOWMAP && LIGHTMAP
      Texture2D   LightMask         : register(t6);
      sampler     LightMaskSampler  : register(s6);
    $endif
  $endif
 
  #ifdef USE_SPECMAP
    Texture2D SpecularMap           : register(t5);
    sampler   SpecularMapSampler    : register(s5);
  #endif

#else
  sampler2D   BaseTexture         : register(s0);
  
  #if defined(USE_NORMALMAP)
    sampler2D   NormalMap           : register(s1);
  #endif
  
  $ifdef LIGHTMAP
    sampler2D   Lightmap            : register(s2);
  $endif
  
  #if defined(USE_CUBEMAP)
    samplerCUBE CubeMap             : register(s3);
  #endif
  
  $ifdef USE_SHADOWMAP
    sampler2D   ShadowTex           : register(s4);

	$if MOBILE_SUBTRACTIVE_SHADOWMAP && LIGHTMAP
	  sampler2D   LightMask         : register(s6);
    $endif
  $endif
  
  #if defined(USE_SPECMAP)
    sampler2D   SpecularMap         : register(s5);
  #endif	
#endif

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4 depthFogCol : packoffset(c20);  // linear depth fog color
	$if USE_GLOBAL_AMBIENT  
      float4 gAmbientCol : packoffset(c21);  // global ambient color
    $endif
  }
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    // x:specular mul
    float4 MaterialParams : packoffset(c0);
	
    $if VMATERIAL_BUMP || VMATERIAL_BUMPANIMATED
      float4 BumpStrength : packoffset(c1);
    $endif
    
    $if VMATERIAL_FAKESPECULAR
      float3 SpecularColor : packoffset(c1);
    $endif
    
    $if MOBILE_DYNAMICBASEPASSLIGHTING && !MOBILE_VERTEXLIGHTING
      float4 LightRadius : packoffset(c2);
      float4 LightColor : packoffset(c3);    
      $if DIRECTIONALLIGHT || SPOTLIGHT
        float4 LightDir : packoffset(c4);
      $endif
      $if SPOTLIGHT
        float4 LightAngle : packoffset(c5);
      $endif
    $endif
    
    $if USE_SHADOWMAP
      float4 ShadowParameters : packoffset(c6);
	  float4 ShadowColor      : packoffset(c7);
    $endif

    $if VMATERIAL_GLOW
      float GlowIntensity : packoffset(c8);
    $endif
    
    $ifdef ALPHATEST
      float AlphaThreshold : packoffset(c9);
    $endif
  }
  
  cbuffer g_GlobalConstantBufferLightGrid : register (b3)
  {
    $ifdef LIGHTGRIDSIMPLE
      float4 AmbientColor : packoffset(c0);
      float4 DiffuseColor : packoffset(c1);
    $endif
  }  
#else
  float4 depthFogCol : register(c1);  // linear depth fog color
  
  $if USE_GLOBAL_AMBIENT
    float3 gAmbientCol : register(c21); // global ambient
  $endif
  
  $ifdef LIGHTGRIDSIMPLE
    float4 AmbientColor : register(c26);
    float4 DiffuseColor : register(c27);
  $endif
  
  float4 MaterialParams : register(c32);

  $if VMATERIAL_BUMP || VMATERIAL_BUMPANIMATED
    float4 BumpStrength : register(c33);
  $endif
  
  $if VMATERIAL_FAKESPECULAR
    float3 SpecularColor : register(c33);
  $endif
  
  $if MOBILE_DYNAMICBASEPASSLIGHTING && !MOBILE_VERTEXLIGHTING
    float4 LightRadius : register(c67);
    float4 LightColor : register(c68); 
    $if DIRECTIONALLIGHT || SPOTLIGHT
      float4 LightDir : register(c69);
    $endif
    $if SPOTLIGHT
      float4 LightAngle : register(c70);
    $endif
  $endif
  
  $if USE_SHADOWMAP
    float4 ShadowParameters : register(c71);
	float4 ShadowColor : register(c72);
  $endif  

  $if VMATERIAL_GLOW
    float GlowIntensity : register(c73);
  $endif
  
  $ifdef ALPHATEST
    float AlphaThreshold : register(c74);
  $endif

#endif

$if (MOBILE_DYNAMICBASEPASSLIGHTING && !MOBILE_VERTEXLIGHTING) 
  #include "DynamicLightingEquationsMobile.inc"
$endif

$if USE_SHADOWMAP
  #include "ShadowMapMobileHelpers.inc" 
$endif

FIXED4 ps_main(PS_IN In) : SV_Target
{
  half2 BaseMapCoord = In.UV0.xy;
  
  FIXED4 BaseTex = FIXED4(vTex2D(BaseTexture, BaseTextureSampler, BaseMapCoord));
  FIXED3 BaseColor = BaseTex.xyz;
  
  $ifdef ALPHATEST
    clip(BaseTex.a - AlphaThreshold);
  $endif
  
  // Fetch the tangent space normal from the normal map if required
  #if defined(USE_NORMALMAP)
    $if VMATERIAL_BUMPANIMATED
      half3 BumpNormalTS = half3(vTex2D(NormalMap, NormalMapSampler, In.UV1.xy).xyz) * 2.0h - 1.0h;
      BumpNormalTS = lerp(half3(0.0h, 0.0h, 1.0h), BumpNormalTS, half(BumpStrength.x));
      half3 BumpNormalTS2 = half3(vTex2D(NormalMap, NormalMapSampler, In.UV1.zw).xyz) * 2.0h - 1.0h;
      BumpNormalTS += lerp(half3(0.0, 0.0, 1.0), BumpNormalTS, half(BumpStrength.y));
      BumpNormalTS = normalize(BumpNormalTS);
    $else
      half3 BumpNormalTS = half3(vTex2D(NormalMap, NormalMapSampler, BaseMapCoord).xyz) * 2.0h - 1.0h;
    $endif
  #endif
  
  #if defined(CALCULATE_BINORMAL_PS)
    half3 BiNormalWS = cross(In.TangentWS.xyz, In.NormalWS)*In.TangentWS.w;
  #endif

  // If we need world space normals, transform the normal map value if possible, or take the interpolated normal otherwise
#if defined(USE_NORMALS_PS)
  #if defined(USE_TANGENTSPACE_PS) && defined(USE_NORMALMAP)
    half3 BumpNormalWS = BumpNormalTS.x * In.TangentWS - BumpNormalTS.y * BiNormalWS + BumpNormalTS.z * In.NormalWS;
  #else
    half3 BumpNormalWS = In.NormalWS;
  #endif
#endif

  // Base lighting: Use the specified lighting method
$if LIGHTGRIDSIMPLE
  $if VMATERIAL_BUMP
    // For simple bump lighting, we transformed the light direction to tangent space in the VS, so we can use that here.
    FIXED Diffuse = max(FIXED(dot(BumpNormalTS, In.LightDirFogCoord.xyz)), FIXED(0.0));
  $else
	  // For all other techniques we either use the geometric normal, or we have to transform the normal map to worldspace anyway,
	  // so there is no saving from transforming the light direction in the vertex shader first.
    FIXED Diffuse = max(FIXED(dot(BumpNormalWS, In.LightDirFogCoord.xyz)), FIXED(0.0));
  $endif
  
  FIXED3 StaticLightColor = FIXED3(AmbientColor.xyz) + FIXED3(DiffuseColor.xyz) * Diffuse;
$elif LIGHTMAP
  FIXED3 StaticLightColor = vTex2D(Lightmap, LightmapSampler, In.UV1.xy).xyz * 2.0h;
$elif FULLBRIGHT
  FIXED3 StaticLightColor = FIXED3(1, 1, 1);
$else		// Dynamic only
  FIXED3 StaticLightColor = FIXED3(0, 0, 0);
$endif
  
  // Bump lighting uses a fake global light direction which is transformed to tangent space
  // and then combined with the normal map to simulate the normal map's effect. In lightgrid mode,
  // we have a "proper" diffuse light direction and hence don't have to use the fake direction.
$if VMATERIAL_BUMP && LIGHTMAP
  StaticLightColor += FIXED3((dot(BumpNormalTS, In.LightDirFogCoord.xyz) - In.LightDirFogCoord.z) * BumpStrength.x * StaticLightColor);
$endif
	
$if VMATERIAL_CUBEMAP || VMATERIAL_BUMPCUBEMAP || VMATERIAL_FAKESPECULAR
  // normalize eye direction vector via interpolated w-component 
	half3 ReflectionVector = reflect(half3(In.EyeDirWS.xyz/In.EyeDirWS.w), BumpNormalWS);
$endif
  
  // Add cubemap reflections to the base color instead of the light color so that they are affected by the static lighting,
  // which lets them better blend in with the rest of the scene.
$if VMATERIAL_CUBEMAP || VMATERIAL_BUMPCUBEMAP
  BaseColor += vTexCUBE(CubeMap, CubeMapSampler, ReflectionVector).xyz * BaseTex.www;
$endif

$if VMATERIAL_FAKESPECULAR

  $if VMATERIAL_REFMAP
    half3 Specular = vTex2D(SpecularMap, SpecularMapSampler, BaseMapCoord).xyz * MaterialParams.x;
  $else
    half3 Specular = SpecularColor * MaterialParams.x;
  $endif
  
  BaseColor += vTexCUBE(CubeMap, CubeMapSampler, ReflectionVector).xyz * Specular;
$endif

$if USE_SHADOWMAP
  half shadowTerm = ComputeShadowTerm(In.ShadowCoords);
  $if DIRECTIONALLIGHT
    shadowTerm = FadeoutShadow(shadowTerm, In.EyeDirWS.w, ShadowParameters.xy);
  $endif 
$endif 

$if MOBILE_SUBTRACTIVE_SHADOWMAP
  FIXED3 vMult = ShadowColor + shadowTerm * (1.0f - ShadowColor);

  $if LIGHTMAP
	FIXED lightMask = 1.0f - vTex2D(LightMask, LightMaskSampler, In.UV1.xy).r;
	vMult = max(vMult, lightMask.xxx);
  $endif

  StaticLightColor *= vMult;    
$endif 
 
FIXED3 Color = (BaseColor * StaticLightColor);
  
$if USE_GLOBAL_AMBIENT
  Color += BaseTex.xyz * FIXED3(gAmbientCol.xyz);
$endif  
  
$if VMATERIAL_GLOW
  Color += BaseColor.xyz * BaseTex.aaa * GlowIntensity;
$endif
  
$if MOBILE_DYNAMICBASEPASSLIGHTING

  $if MOBILE_VERTEXLIGHTING
  
    // get interpolated color
    half4 DynamicLightColor = In.VertexColor;
    
  $elif MOBILE_DYNAMICLIGHTNORMALMAPPING
  
    $if DIRECTIONALLIGHT || SPOTLIGHT
      half3 LightDirTS = GetVectorInTextureSpace(-LightDir.xyz, In.TangentWS, In.NormalWS, BiNormalWS);
    $endif
    
    $if DIRECTIONALLIGHT
      half4 DynamicLightColor = ComputeMobileLightingDirectional(LightDirTS, BumpNormalTS);
    $elif POINTLIGHT
      half4 DynamicLightColor = ComputeMobileLightingPoint(In.vLightVec, BumpNormalTS);
    $elif SPOTLIGHT
      half4 DynamicLightColor = ComputeMobileLightingSpot(In.vLightVec, LightDirTS, BumpNormalTS);
    $endif
    
  $else
  
    $if DIRECTIONALLIGHT
      half4 DynamicLightColor = ComputeMobileLightingDirectional(-LightDir.xyz, normalize(In.NormalWS));
    $elif POINTLIGHT
      half4 DynamicLightColor = ComputeMobileLightingPoint(In.vLightVec, normalize(In.NormalWS));
    $elif SPOTLIGHT
      half4 DynamicLightColor = ComputeMobileLightingSpot(In.vLightVec, -LightDir.xyz, normalize(In.NormalWS));
    $endif
    
  $endif
  
  $if USE_SHADOWMAP
    DynamicLightColor.xyz *= shadowTerm;
  $endif
  
  Color += BaseTex.xyz * FIXED3(DynamicLightColor.xyz);
$endif 

$ifndef NOFOG
  Color = GetFogColor(Color, depthFogCol.xyz, In.LightDirFogCoord.w);
$endif

  return FIXED4(Color, BaseTex.w);
}
