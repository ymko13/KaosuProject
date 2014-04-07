
#ifndef VISION_SHADER_HELPERS
#define VISION_SHADER_HELPERS


struct LIGHT_RESULT_SPEC
{
  float4 diffuse;
  float4 specular;
};

// uncomment this and recompile the shader to get energy conservation for specular highlights.
//#define USE_ENERGY_CONSERVATION

#ifdef _VISION_DX11
  float GetFacing(bool isFrontFace)
  {
    return isFrontFace.x ? 1.0f : -1.0f;
  }
#else
  float GetFacing(float facing)
  {
    #if defined(_VISION_XENON) || defined(_VISION_PS3)
      return facing.x;
    #else
      return -facing.x;
    #endif
  }
#endif

float GetHandednessCorrection(float3 UntransformedTangent)
{
  return dot(UntransformedTangent, UntransformedTangent) - 2.0f;
}

float3 ComputeBiNormal(float3 Tangent, float3 Normal, float HandednessCorrection)
{
  return normalize(cross(Tangent, Normal) * HandednessCorrection);
}

float3 GetVectorInTextureSpace(float3 InVector, float3 Tangent, float3 Normal, float3 BiNormal)
{
  float3 OutVector;
  OutVector.x = dot (Tangent, InVector);
  OutVector.y = -dot (BiNormal, InVector);
  OutVector.z = dot (Normal, InVector);
  return OutVector;
}

float3x3 GetInverseTextureSpace(float3 Tangent, float3 Normal, float3 BiNormal)
{
  float3x3 mat;
#ifdef _VISION_WIIU
  mat._m00 = -Tangent.x;
  mat._m10 = -Tangent.y;
  mat._m20 = -Tangent.z;
  
  mat._m01 = BiNormal.x;
  mat._m11 = BiNormal.y;
  mat._m21 = BiNormal.z;
  
  mat._m02 = -Normal.x;
  mat._m12 = -Normal.y;
  mat._m22 = -Normal.z;
#else
  mat._m00_m10_m20 = -Tangent;
  mat._m01_m11_m21 = BiNormal;
  mat._m02_m12_m22 = -Normal;
#endif

  return mat;
}

float3 TangentToWorldSpace(float3 tangentSpaceVec, float3 Normal, float3 Tangent, float3 BiNormal)
{
  return tangentSpaceVec.r * Tangent - tangentSpaceVec.g * BiNormal + tangentSpaceVec.b * Normal;
}

// by defining this preprocessor tag, a custom normal decoding can be supplied. 
#ifndef VMATERIAL_CUSTOMNORMALMAPDECODINGFUNCTION

// Global function to extract the (unnormalized) normal vector and alpha from incoming normal map sampler
float4 DecodeNormalAndAlpha(float4 normalmap)
{
  normalmap.xyz = normalmap.xyz * 2.0 - 1.0;
  return normalmap;
}

#endif

float GetBlinnPhongNormalizationFactor(float specExp)
{
  return (8 + specExp) * 0.039789f; //< approx for 1/(8*pi)
}

//Note: HalfAngle and Normal should be in the same space
float GetSpecularIlluminationPoint(float3 HalfAngle, float3 Normal, float Exp)
{
    float SpecProd;
    
    float3 nrmHalfAngleVec = normalize(HalfAngle);
    SpecProd = dot(nrmHalfAngleVec, Normal);
    SpecProd = pow(max(1e-3, SpecProd), Exp);
  #ifdef USE_ENERGY_CONSERVATION
    SpecProd *= GetBlinnPhongNormalizationFactor(Exp);
  #endif
    return SpecProd;
}

//transform the normal map normal into world space by using the tangent space
float3 GetWorldNormal(float4 cNormalMap, float3 Tangent, float3 BiTangent, float3 Normal)
{
  return normalize(cNormalMap.x*Tangent -
                   cNormalMap.y*BiTangent + 
                   cNormalMap.z*Normal);
}



//Calculates the lightgrid lighting of a worldspace normal
//Returns R,G,B,1 so we can multiply with a color and keep
//the alpha.

float4 GetLightGridDiffuse_WS(float3 worldNormal,
                              float4 Light0, float4 Light1,
                              float4 Light2, float4 Light3,
                              float4 Light4, float4 Light5
                             )
{
  float3 w = worldNormal*worldNormal;

  float4 vAccum;

  if (worldNormal.x>0)
    vAccum = Light0*w.x;
  else
    vAccum = Light1*w.x;

  if (worldNormal.y>0)
    vAccum += Light2*w.y;
  else
    vAccum += Light3*w.y;

  if (worldNormal.z>0)
    vAccum += Light4*w.z;
  else
    vAccum += Light5*w.z;

  return float4(vAccum.xyz,1);
}


//Computes the lightgrid diffuse+specular for a worldspace normal
LIGHT_RESULT_SPEC GetLightGridSpec_WS(float3 eyeDir, float3 normal, 
                                      float4 Light0, float4 Light1, 
                                      float4 Light2, float4 Light3, 
                                      float4 Light4, float4 Light5, 
                                      float specExp)
{
  LIGHT_RESULT_SPEC lsres;
  
  float3 w = normal*normal;
  
  float3 vHalfAngleX,vHalfAngleY,vHalfAngleZ;
  float4 cDiffuseX,cDiffuseY,cDiffuseZ;
  
  if (normal.x>0.0f)
  {
    vHalfAngleX = float3(1,0,0);
    cDiffuseX = Light0;
  } 
  else
  {
    vHalfAngleX = float3(-1,0,0);
    cDiffuseX = Light1;
  }
  
  if (normal.y>0.0f)
  {
    vHalfAngleY = float3(0,1,0);
    cDiffuseY = Light2;
  } 
  else
  {
    vHalfAngleY = float3(0,-1,0);
    cDiffuseY = Light3;
  }
  
  if (normal.z>0.0f)
  {
    vHalfAngleZ = float3(0,0,1);
    cDiffuseZ = Light4;
  } 
  else
  {
    vHalfAngleZ = float3(0,0,-1);
    cDiffuseZ = Light5;
  }
  
  vHalfAngleX += eyeDir;
  vHalfAngleY += eyeDir;
  vHalfAngleZ += eyeDir;
  
  //Compute the specular
  float3 vSpec;
  vSpec.x = dot(normalize(vHalfAngleX), normal);
  vSpec.y = dot(normalize(vHalfAngleY), normal);
  vSpec.z = dot(normalize(vHalfAngleZ), normal);
  
  vSpec = pow(max((float3)1e-3f, vSpec), specExp);
  #ifdef USE_ENERGY_CONSERVATION
    vSpec *= GetBlinnPhongNormalizationFactor(specExp);
  #endif
  
  cDiffuseX *= w.x;
  cDiffuseY *= w.y;
  cDiffuseZ *= w.z;
  
  lsres.diffuse  = cDiffuseX          +  cDiffuseY         + cDiffuseZ;
  lsres.specular = cDiffuseX*vSpec.x  +  cDiffuseY*vSpec.y + cDiffuseZ*vSpec.z;
  
  return lsres;
}


//Computes the lighting on one axis of the lightgrid
//Used by the SpeedTree Shaders
float4 GetLightGridDiffuse(float3 lightDir, float3 normal, float4 Light0, float4 Light1)
{

  float fDot = dot(lightDir, normal);
  float4 lcolor;
  if (fDot>0.0f)
  {
    lcolor = Light0;
  } 
  else
  {
    lcolor = Light1;
  }
  return lcolor*fDot*fDot;
}

void GetLightmapDot3Base(out float3 vLightDir1, out float3 vLightDir2, out float3 vLightDir3)
{
  vLightDir1 = float3(-0.816497, -0.000000, 0.577350);
  vLightDir2 = float3(0.408248,  -0.707107, 0.577350);
  vLightDir3 = float3(0.408248,   0.707107, 0.577350);
}

//Computes the lightmapped diffuse for a tangentspace normal
float3 GetLightmapDot3LightingDiffuse(float3 vTangentSpaceNormal, float4 cLightmap1, float4 cLightmap2, float4 cLightmap3)
{
  float3 vLightDir1, vLightDir2, vLightDir3;
  GetLightmapDot3Base(vLightDir1, vLightDir2, vLightDir3);

  // direction 1
  float fDot = saturate(dot(vTangentSpaceNormal, vLightDir1));
  float4 vAccum = cLightmap1 * fDot;

  // direction 2
  fDot = saturate(dot(vTangentSpaceNormal, vLightDir2));
  vAccum += cLightmap2 * fDot;

  // direction 2
  fDot = saturate(dot(vTangentSpaceNormal, vLightDir3));
  vAccum += cLightmap3 * fDot;

  return vAccum.xyz;
}

LIGHT_RESULT_SPEC ComputeDot3LightmapLightingSpec(float3 normal, float3 lightDir, float3 eyeDir, float4 lightContrib, float specExp)
{
  LIGHT_RESULT_SPEC Out;
  float fNDotL = saturate(dot(normal, lightDir));
  Out.diffuse = lightContrib * fNDotL;
  
  float3 halfAngle = normalize(eyeDir + lightDir);
  float fNDotH = saturate(dot(normal, halfAngle));
  Out.specular = lightContrib * fNDotL * pow(max(1e-3f, fNDotH), specExp);
  #ifdef USE_ENERGY_CONSERVATION
    Out.specular *= GetBlinnPhongNormalizationFactor(specExp);
  #endif
  return Out;
}

//Computes the lightmapped diffuse+specular for a tangentspace normal
LIGHT_RESULT_SPEC GetLightmapDot3LightingSpec(float3 vTangentSpaceNormal, float4 cLightmap1, float4 cLightmap2, float4 cLightmap3, float3 eyeDir, float fSpecExp)
{
  float3 vLightDir1, vLightDir2, vLightDir3;
  GetLightmapDot3Base(vLightDir1, vLightDir2, vLightDir3);
  LIGHT_RESULT_SPEC Out;
  Out = ComputeDot3LightmapLightingSpec(vTangentSpaceNormal, vLightDir1, eyeDir, cLightmap1, fSpecExp);

  LIGHT_RESULT_SPEC lsres;
  lsres = ComputeDot3LightmapLightingSpec(vTangentSpaceNormal, vLightDir2, eyeDir, cLightmap2, fSpecExp);
  Out.diffuse  += lsres.diffuse;
  Out.specular += lsres.specular;

  lsres = ComputeDot3LightmapLightingSpec(vTangentSpaceNormal, vLightDir3, eyeDir, cLightmap3, fSpecExp);
  Out.diffuse  += lsres.diffuse;
  Out.specular += lsres.specular;
  
  return Out;
}

// maps the eyespace distance to [0..1]
float GetFogCoord(float4 fog, float z)
{
  return (z-fog.x) * fog.z;
}

// weightens the color with the fog color using the fog coordinate
float3 GetFogColor(float3 color, float3 fogCol, float fogCoord)
{
  return lerp(color.xyz, fogCol.xyz, saturate(fogCoord));
}

#if defined(_VISION_DX9) || defined(_VISION_XENON)
  #define GetScreenTexPos(screenPos, invScreenSize) (((screenPos.xy / screenPos.w) + invScreenSize.xy) * 0.5f + 0.5f)
#else
  #define GetScreenTexPos(screenPos, invScreenSize) ((screenPos.xy / screenPos.w) * 0.5f + 0.5f)
#endif

inline float GetLinearDepth(float viewSpaceDepth, float farClippingPlane)
{
  float depth = viewSpaceDepth / farClippingPlane;
  return depth;
}

//this always returns the positive linear depth value in eye space (in world units)
//input value directly from the shadowmap or the projection.z coordinate
float DepthTextureToLinearDepth(float depthValue, float nearClippingPlane, float farClippingPlane)
{
  #ifdef _VISION_PS3
  depthValue = depthValue * 0.5f + 0.5f;
  #endif
  return (farClippingPlane * nearClippingPlane) / ( depthValue * (nearClippingPlane - farClippingPlane) + farClippingPlane);
}

// converts linear RGB to sRGB
float3 LinearToSRGB(float3 linRGB)
{
  float eps = 0.0031308;
  float a = 0.055;
  return (linRGB <= eps) ? (12.92 * linRGB) : ((1 + a) * pow(linRGB, 1.0 / 2.4) - a);
}

#endif
