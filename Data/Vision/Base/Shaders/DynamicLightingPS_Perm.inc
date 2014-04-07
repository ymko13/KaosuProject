#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <shaders/vMaterialPermutations.inc>
#include <shaders/SkinShadingHelper.inc>

#ifdef _VISION_DX11
  Texture2D   BaseTexture         : register(t0);
  sampler     BaseTextureSampler  : register(s0);

  Texture2D   AttenTex            : register(t1);
  sampler     AttenTexSampler     : register(s1);
  
  $ifdef VMATERIAL_NORMALMAP
    Texture2D   NormalMap         : register(t2);
    sampler     NormalMapSampler  : register(s2);
  $endif

  $ifdef DYNLIGHT_OMNI_PROJECTOR
    TextureCube ProjTex           : register(t3);
    sampler     ProjTexSampler    : register(s3);
  $endif
  $ifdef DYNLIGHT_SPOT_PROJECTOR
    Texture2D   ProjTex           : register(t3);
    sampler     ProjTexSampler    : register(s3);
  $endif

  $ifdef VMATERIAL_REFMAP
    Texture2D   RefMap            : register(t4);
    sampler     RefMapSampler     : register(s4);
  $endif

  $ifdef SHADOWMAP
    Texture2D   ShadowTex         : register(t5);
    sampler     ShadowTexSampler  : register(s5);
	$if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	Texture2D	RealShadowMap        : register(t7);
	sampler     RealShadowMapSampler : register(s7);
	$endif
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	Texture2D	BeckmannLookup    : register(t6);
	sampler		BeckmannLookupSampler : register(s6);
  $endif
  
  #ifdef VMATERIAL_SKIN_TRANSMITTANCE
	Texture2D	SkinTransmittance    		: register(t8);
	sampler		SkinTransmittanceSampler    : register(s8);
  #endif
  
  $if defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	Texture2D	skinDiffuseScattering    : register(t9);
	sampler		skinDiffuseScatteringSampler : register(s9);
	Texture2D	skinShadowScattering    : register(t10);
	sampler		skinShadowScatteringSampler : register(s10);
  $endif

#else
  sampler2D   BaseTexture         : register(s0);   //Base texture
  sampler2D   AttenTex            : register(s1);   //Attenuation curve
  
  $ifdef VMATERIAL_NORMALMAP
    sampler2D   NormalMap         : register(s2);
  $endif

  $ifdef DYNLIGHT_OMNI_PROJECTOR
    samplerCUBE ProjTex           : register(s3);
  $endif
  $ifdef DYNLIGHT_SPOT_PROJECTOR
    sampler2D   ProjTex           : register(s3);
  $endif

  $ifdef VMATERIAL_REFMAP
    sampler2D   RefMap            : register(s4);
  $endif
  
  $ifdef SHADOWMAP
    sampler2D   ShadowTex         : register(s5);
	#ifdef VMATERIAL_SKIN_TRANSMITTANCE
	sampler2D   RealShadowMap     : register(s7);
	#endif
  $endif
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	sampler2D	BeckmannLookup    : register(s6);
  $endif

  #ifdef VMATERIAL_SKIN_TRANSMITTANCE
	sampler2D	SkinTransmittance    : register(s8);
  #endif
  
  $if defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	sampler2D	skinDiffuseScattering   : register(s9);
	sampler2D	skinShadowScattering    : register(s10);
  $endif
#endif

#if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)

  float4 MaterialParams : register(c32);
  float4 LightColor     : register(c33);
  float4 LightRadius    : register(c34);
  
  $ifdef DYNLIGHT_DIRECTIONAL
    float3 LightDirPS   : register(c35);
  $endif  
  $ifdef SHADOWMAP
    float2 InvScreenSize : register(c36);
  $endif  
  
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
    $ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
      float4 SkinShadingColor  : register(c37);
	$endif
    float4 SkinShadingParams : register(c38); // x = rho_s, y = m, z = tuneCurvature, w = shadow bleeding modifier
	$ifdef VMATERIAL_SKIN_SHADING_PREINTEGRATED
	float3 NormalMapSize : register(c39);
	$endif
  $endif
  
  #ifdef VMATERIAL_SKIN_TRANSMITTANCE
  float4x4 LightProjectionTex[4] : register(c40); //matrices for all 4 cascades
  float4 CascadeScale : register(c56); //scaleing for all 4 cascades
  float4 FarClips : register(c57);  //near clipping values for all 4 cascades
  float4 NearClips : register(c58); //far clipping values for all 4 cascades
  float4 CascadeTextureSplitsEnd : register(c61);
  float4 CascadeBias : register(c62);
  float4 TransmittanceParams : register(c63); // x = Transmittance multiplier, y = transmittance artifact avoidance, z = maximum transmittance thickness, w = minimum transmittance tickness
  #endif

  float4 EyePlane : register(c59); 
  float4 EyePos : register(c60);
  
#elif defined(_VISION_DX11)
  
  cbuffer g_GlobalConstantBufferUser : register (b2)
  {
    float4 MaterialParams : packoffset(c0); // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself!
    float4 LightRadius    : packoffset(c1); // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
    float4 LightColor     : packoffset(c2); // xyz: color with pre-multiplied light multiplier
    
    $ifdef DYNLIGHT_DIRECTIONAL
      float3 LightDirPS   : packoffset(c3);
    $endif  
    $ifdef SHADOWMAP
      float2 InvScreenSize : packoffset(c4);
    $endif  
	
    $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
        float4 SkinShadingColor  : packoffset(c5);
	  $endif
      float4 SkinShadingParams : packoffset(c6); // x = rho_s, y = m, z = tuneCurvature, w = shadow bleeding modifier
	  $ifdef VMATERIAL_SKIN_SHADING_PREINTEGRATED
	  float3 NormalMapSize : packoffset(c7);
	  $endif
    $endif
    #ifdef VMATERIAL_SKIN_TRANSMITTANCE
	  float4x4 LightProjectionTex[4] : packoffset(c16); //matrices for all 4 cascades
	  float4 CascadeScale : packoffset(c8); //scaleing for all 4 cascades
	  float4 FarClips : packoffset(c9);  //near clipping values for all 4 cascades
	  float4 NearClips : packoffset(c10); //far clipping values for all 4 cascades
	  float4 CascadeTextureSplitsEnd : packoffset(c13);
	  float4 CascadeBias : packoffset(c14);
	  float4 TransmittanceParams : packoffset(c15); // x = Transmittance multiplier, y = transmittance artifact avoidance, z = maximum transmittance thickness, w = minimum transmittance tickness
    #endif

	float4 EyePlane : packoffset(c11); 
    float4 EyePos : packoffset(c12);
  }
#else
  float4 MaterialParams; // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself!
  float4 LightRadius;    // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
  float4 LightColor;     // xyz: color with pre-multiplied light multiplier
  
  $ifdef DYNLIGHT_DIRECTIONAL
    float3 LightDirPS;
  $endif
  $ifdef SHADOWMAP
    float2 InvScreenSize;
  $endif    
  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	  $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
		float4 SkinShadingColor;
	  $endif
	  float4 SkinShadingParams; // x = rho_s, y = m, z = tuneCurvature, w = shadow bleeding modifier
	  $ifdef VMATERIAL_SKIN_SHADING_PREINTEGRATED
	  float3 NormalMapSize;
	  $endif
  $endif
  #ifdef VMATERIAL_SKIN_TRANSMITTANCE
  float4x4 LightProjectionTex[4]; //matrices for all 4 cascades
  float4 CascadeScale; //scaleing for all 4 cascades
  float4 FarClips;  //near clipping values for all 4 cascades
  float4 NearClips; //far clipping values for all 4 cascades
  float4 CascadeTextureSplitsEnd;
  float4 CascadeBias;
  float4 TransmittanceParams; // x = Transmittance multiplier, y = transmittance artifact avoidance, z = maximum transmittance thickness, w = minimum transmittance tickness
  #endif

  float4 EyePlane; 
  float4 EyePos;
#endif


$if (defined (DYNLIGHT_SPOT_PROJECTOR) || defined (DYNLIGHT_OMNI_PROJECTOR)) && defined (VMATERIAL_NORMALMAP) && defined (VMATERIAL_PARALLAX) && defined (VMATERIAL_SPECULAR)
	V_REQUIRES_SM30;
$endif
      
  struct PS_IN                                 
  {                                             
    float4 ProjPos      : SV_Position;
    
    $ifdef SHADOWMAP
      float4 UV0 : TEXCOORD0;     // u, v, screenX, screenY
    $else
      float2 UV0          : TEXCOORD0;
    $endif

  $ifdef USE_FOG
    float4 Normal  : TEXCOORD1; // encode fogcoord into w component because of limited amount of intepolators
  $else
    float3 Normal  : TEXCOORD1;
  $endif
    
    $ifdef VMATERIAL_NORMALMAP
      float3 Tangent      : TEXCOORD2;
      float3 BiTangent    : TEXCOORD3;
    $endif
    
    $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
      float3 VWorldPos : TEXCOORD4;
    $elif defined(VMATERIAL_SPECULAR)
      float3 HalfAngleWS : TEXCOORD4;
    $endif
    
    $ifdef SHADOWMAP
      float4 LightWS : TEXCOORD5;  // Lx, Ly, Lz, screenW
    $else
      float3 LightWS      : TEXCOORD5;
    $endif
    
    $ifdef DYNLIGHT_OMNI_PROJECTOR
      float3 ProjDir      : TEXCOORD6;
    $endif
    $ifdef DYNLIGHT_SPOT_PROJECTOR
      float4 ProjTexCoord : TEXCOORD6;
    $endif

    $if defined(VMATERIAL_PARALLAX) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
      float3 EyeDir       : TEXCOORD7;
    $endif
  };
  
#include <shaders/vMaterialHelpers.inc>

//Neccessary defines for cascade selection (not mentioned lights don't have any)
#ifdef VMATERIAL_SKIN_TRANSMITTANCE
	#if defined(POINT_LIGHT)
	#define SHADOWING_PROJECTION_POINT
	#elif defined(DIR_LIGHT)
	#define MAX_CASCADES 4
	#define SHADOWING_SPLITMODE_INTERVAL_DEPTH
	#define SHADOWING_CASCADED
	#else //SPOT_LIGHT
	#define MAX_CASCADES 1
	#endif
	static float3 LightWorldSpace;

	#include <Shaders/ShadowMapHelpers.inc>
#endif

float4 ps_main( PS_IN In ) : SV_Target
{
  $ifdef DYNLIGHT_DIRECTIONAL
    float LightInt = 1;
  $elif DYNLIGHT_ATTENUATION_ONE_OVER_X
    float LightInt = rsqrt(dot(In.LightWS, In.LightWS));
    LightInt = saturate(LightInt*LightRadius.x - LightRadius.w);
  $else
    //Custom attenuation curve
    //float LightIntR = sqrt(dot(In.LightWS.xyz, In.LightWS.xyz)) * LightRadius.y;  // dist 0..1 for 0..r. Let the sampler clamp it to [0..1], not the shader
  
    float LightIntR = length(In.LightWS.xyz) * LightRadius.y;  // dist 0..1 for 0..r. Let the sampler clamp it to [0..1], not the shader
    LightIntR = saturate(LightIntR);

    #if defined(_VISION_GLES2)
      float LightInt = saturate(1.0 - LightIntR); //custom attenuation curve texture
    #else
      float LightInt = vTex2D(AttenTex, AttenTexSampler, float2(LightIntR, 0.0f)).x; //custom attenuation curve texture
    #endif
  $endif
  
  $ifdef DYNLIGHT_OMNI_PROJECTOR
    $ifdef DYNLIGHT_OMNI_PROJECTOR_DISTANCEMIPMAP
      // use a lower mipmap according to distance (assuming hardcoded first 4 miplevels)
      float4 ProjLightCol = LightColor*vTexCUBEbias(ProjTex, ProjTexSampler, float4(In.ProjDir,LightIntR*4.0));
    $else
      float4 ProjLightCol = LightColor*vTexCUBE(ProjTex, ProjTexSampler, In.ProjDir); // Cubemap projector
    $endif
  $elif defined(DYNLIGHT_SPOT_PROJECTOR)

    float4 ProjLightCol = LightColor * 
    #ifdef _VISION_PSP2
      // Use explicit clamping as the PSVita doesn't clamp correctly here.
      // TODO: check if this is fixed in future SDKs or find out how to use tex2Dproj for the PSVita
      tex2D(ProjTex, saturate(In.ProjTexCoord.xy / In.ProjTexCoord.w));
    #else
      vTex2Dproj(ProjTex, ProjTexSampler, In.ProjTexCoord); //2D spotlight projector
    #endif

  $else
    float4 ProjLightCol = LightColor; //No projector
  $endif

  $ifdef VMATERIAL_PARALLAX
    float3 vNormEyeTS = normalize(In.EyeDir);
	float2 BaseMapCoord = GetBaseMapCoords(In.UV0.xy, vNormEyeTS);
  $else
    float2 BaseMapCoord = In.UV0.xy;
  $endif

  float4 BaseMapValue = GetDiffuseTexture(BaseMapCoord);
  
  float4 NormalMapValue = GetTangentSpaceNormal(BaseMapCoord);
  $ifdef VMATERIAL_SPECULAR
    float4 SpecMul = GetSpecularMultiplier(BaseMapCoord, NormalMapValue.a);
    float fSpecExp = SpecMul.a;
  $endif
  
  $ifdef VMATERIAL_NORMALMAP
    NormalMapValue.xyz = GetWorldNormal(NormalMapValue, In.Tangent, In.BiTangent, In.Normal.xyz);
  $else  
    NormalMapValue = float4(normalize(In.Normal.xyz),1.f);
  $endif

  $ifdef DYNLIGHT_DIRECTIONAL
    float3 NormLightVec = -LightDirPS;
  $else
    float3 NormLightVec = normalize(In.LightWS.xyz);
  $endif
 
  $ifdef VMATERIAL_SKIN_SHADING_BLENDED_NORMALS
  float4 DotProd = lerp(float4(1.0f,1.0f,1.0f,1.0f) * saturate(dot(NormLightVec.xyz, normalize(In.Normal))), 
                        float4(1.0f,1.0f,1.0f,1.0f) * saturate(dot(NormLightVec.xyz, NormalMapValue.xyz)),
						SkinShadingColor);
  $else
  float DotProd = saturate(dot(NormLightVec.xyz, NormalMapValue.xyz));
  $endif

  $ifdef VMATERIAL_SKIN_SHADING_PREINTEGRATED
    float3 GeometryNormal = normalize(In.Normal.xyz);
    float curvature = saturate(length(fwidth(GeometryNormal)) / length(fwidth(In.VWorldPos)) * SkinShadingParams.z);
	//float penumbraWidth = 1.0f - (DotProd + saturate(dot(-NormLightVec.xyz, NormalMapValue.xyz)));
	float lod = MipMapLevel(BaseMapCoord.xy, NormalMapSize.xy) + 2.0f;
	float3 N_high = NormalMapValue.xyz;
	float3 N_low = normalize(vTex2Dlod(NormalMap, NormalMapSampler, float4(BaseMapCoord.x, BaseMapCoord.y, 0.0f, lod)).xyz * 2.0 - 1.0);
	N_low = GetWorldNormal(float4(N_low,0.0f), In.Tangent, In.BiTangent, In.Normal.xyz);
	float3 rN = normalize(lerp(N_low, N_high, SkinShadingColor.r));
	float3 gN = normalize(lerp(N_low, N_high, SkinShadingColor.g));
	float3 bN = normalize(lerp(N_low, N_high, SkinShadingColor.b));
    float3 SkinColor;
	SkinColor.r = PreintegratedSkinDiffuse( curvature, dot(NormLightVec.xyz, rN), skinDiffuseScattering, skinDiffuseScatteringSampler ).r;
	SkinColor.g = PreintegratedSkinDiffuse( curvature, dot(NormLightVec.xyz, gN), skinDiffuseScattering, skinDiffuseScatteringSampler ).g;
	SkinColor.b = PreintegratedSkinDiffuse( curvature, dot(NormLightVec.xyz, bN), skinDiffuseScattering, skinDiffuseScatteringSampler ).b;
    float4 ResColor = BaseMapValue * LightInt * ProjLightCol * float4(SkinColor,1.0f);
  $else
    float4 ResColor = BaseMapValue * LightInt * ProjLightCol * DotProd;
  $endif
  
  $ifdef VMATERIAL_SPECULAR
    $if defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED) || defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS)
      $ifdef DYNLIGHT_DIRECTIONAL
        float3 HalfAngleWS = In.EyeDir - LightDirPS;
      $else
        float3 HalfAngleWS = In.EyeDir + normalize(In.LightWS.xyz);
      $endif	  
	$else
	  float3 HalfAngleWS = In.HalfAngleWS;
	$endif
	
    $if defined(VMATERIAL_SKIN_SHADING_BLENDED_NORMALS) || defined(VMATERIAL_SKIN_SHADING_SSSSS) || defined(VMATERIAL_SKIN_SHADING_PREINTEGRATED)
	  ResColor += 10.0f * LightInt * KelemenSzirmay_BRDF( normalize(NormalMapValue.xyz), normalize(NormLightVec.xyz), normalize(In.EyeDir.xyz), SkinShadingParams.x, SkinShadingParams.y, BeckmannLookup, BeckmannLookupSampler ) * ProjLightCol;
	$else
      float SpecProd = GetSpecularIlluminationPoint(HalfAngleWS, NormalMapValue.xyz, fSpecExp);
      SpecProd = SpecProd * LightInt * DotProd;
      ResColor += SpecMul * SpecProd * ProjLightCol;
	$endif
  $endif

  $ifdef DYNLIGHT_SPOT_PROJECTOR
    // Remove backprojection
    #ifdef _VISION_PSP2
      // There are artifacts where the W-Plane hits geometry because tex2Dproj is only emulated on the PSVita (see VisionCommon.inc)
      // -> use an epsilon.
      ResColor *= saturate(sign(In.ProjTexCoord.z - 0.05f));
    #else
      ResColor *= saturate(sign(In.ProjTexCoord.w));
    #endif
  $endif

  ResColor.a = BaseMapValue.a;

  $ifdef SHADOWMAP
    float2 ScreenTex = GetScreenTexPos(float4(In.UV0.zw, 0, In.LightWS.w), InvScreenSize);

	float3 shadowAmount;
    #ifdef _VISION_XENON
      // Note: shadow information is only stored in the
      //   green channel, thus we avoid to overwrite the
      //   stencil values in EDRAM on xbox360 in case of
      //   interleaved shadow map rendering
	  shadowAmount = vTex2D(ShadowTex, ShadowTexSampler, ScreenTex).ggg;
    #else
      shadowAmount = vTex2D(ShadowTex, ShadowTexSampler, ScreenTex).rgb;
    #endif
	
	$ifdef VMATERIAL_SKIN_SHADING_PREINTEGRATED
	  float f = saturate(dot(NormLightVec.xyz, GeometryNormal)) * (1.0f - SkinShadingParams.w) + SkinShadingParams.w;
	  ResColor.rgb *= PreintegratedSkinShadow( shadowAmount.r, f , skinShadowScattering, skinShadowScatteringSampler);
	  //ResColor.rgb = shadowAmount;
	$else
	  ResColor.rgb *= shadowAmount;
	$endif
	
	#ifdef VMATERIAL_SKIN_TRANSMITTANCE
		LightWorldSpace = In.LightWS.xyz; //global variable used by shadowmapping
		// Determine cascade - multiple cascades are only used for directional or point lights
		float fCascadeScale = 1.0;
		float4 vProjCoords = 0.0;
		float4 shrinkedPos = float4(In.VWorldPos - TransmittanceParams.y * normalize(In.Normal.xyz), 1.0);
		int iCascadeIndex = GetShadowTextureCoordsAndCascadeScale(shrinkedPos, EyePos, EyePlane, vProjCoords, fCascadeScale);	
		
		#ifndef SHADOWING_CASCADED
			vProjCoords.xyz /= vProjCoords.w;
		#endif
		
		#if defined(_VISION_PS3) || defined(_VISION_PSP2)
		//PS3 and PSP2 can not index a float4
			float fShadowNear = NearClips.x;
			float fShadowFar = FarClips.x;
			#if MAX_CASCADES > 1
			if(iCascadeIndex == 1)
			{
				fShadowNear = NearClips.y;
				fShadowFar = FarClips.y;
			}
				#if MAX_CASCADES > 2
				else if(iCascadeIndex == 2)
				{
					fShadowNear = NearClips.z;
					fShadowFar = FarClips.z;
				}
					#if MAX_CASCADES > 3
					else
					{
						fShadowNear = NearClips.w;
						fShadowFar = FarClips.w;
					}
					#endif
				#endif
			#endif
		#else
		float fShadowNear = NearClips[iCascadeIndex];
		float fShadowFar = FarClips[iCascadeIndex];
		#endif
		
		//the depth value we get from the shadowmap is offset by the CascadeBias, so we have to remove that offset to get consistent thickness values
		#ifdef SHADOWING_CASCADED 
		//on a cascaded lightsource the depth values are already linear
		float fShadowDepth = LOAD_NORMALIZED_DEPTH_FROM_SHADOWMAP(RealShadowMap, RealShadowMapSampler, vProjCoords.xy);
		float fEyeDepth = vProjCoords.z;
		#ifdef _VISION_PS3
		// the native depth range on the ps3 is (-1.0,1.0)
		fShadowDepth = fShadowDepth * 0.5f + 0.5f;
		fEyeDepth = fEyeDepth * 0.5f + 0.5f;
		#endif
		fShadowDepth *= (fShadowFar - fShadowNear) + fShadowNear; // - CascadeBias[iCascadeIndex];
		fEyeDepth *= (fShadowFar - fShadowNear) + fShadowNear;
		#else 
		//TODO revert the effect of the depth bias to get a more percise depth
		float fShadowDepth = DepthTextureToLinearDepth(LOAD_NORMALIZED_DEPTH_FROM_SHADOWMAP(RealShadowMap, RealShadowMapSampler, vProjCoords.xy), fShadowNear, fShadowFar);// - CascadeBias[iCascadeIndex];
		float fEyeDepth = DepthTextureToLinearDepth(vProjCoords.z, fShadowNear, fShadowFar);
		#endif
		float fThickness = max((fEyeDepth - fShadowDepth), 0.0f) + TransmittanceParams.w;
		fThickness = fThickness / TransmittanceParams.z;
		float3 transmittance = vTex2D(SkinTransmittance, SkinTransmittanceSampler, float2(fThickness, 0.0f)).rgb * saturate(0.3f + dot(NormLightVec.xyz, -normalize(In.Normal.xyz))) * TransmittanceParams.x;
		ResColor.rgb += transmittance * BaseMapValue.rgb * ProjLightCol.rgb * LightInt;
	  #endif
	
  $endif
  
  $ifdef USE_FOG
    ResColor.xyz = GetFogColor(ResColor.xyz, float3(0.0f,0.0f,0.0f), In.Normal.w); 
  $endif  
  
  return ResColor;
}

