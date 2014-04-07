//-----------------------------------------------------------------------------------
// DynamicLightingMobileHelpers.inc
//-----------------------------------------------------------------------------------

$if MOBILE_VERTEXLIGHTING
  #if defined(_IS_VERTEX_SHADER)
    #define _COMPUTES_LIGHTING
  #endif
$else
  #if defined(_IS_PIXEL_SHADER)
    #define _COMPUTES_LIGHTING
  #endif
$endif

//-----------------------------------------------------------------------------------
// Shader unit specific lighting constants
//
// If normal mapping is enabled the LightDir constant is needed in the vertex shader
// in order to transform it into tangent space.

#if defined(_VISION_DX11)
  cbuffer g_GlobalConstantBufferUser : register (b2) {
#endif

// Vertex shader
#if defined(_IS_VERTEX_SHADER)

  #if defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU) || defined(_VISION_PS3)
  
    float4 LightPos     : register(c64);
    $if MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir   : register(c67);
    $endif
    $if USE_SHADOWMAP
      float4x4 LightProjectionTex[4] : register(c68); 
    $endif
    
  #elif defined(_VISION_DX11)
  
    float4 LightPos     : packoffset(c0); 
    $if MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir   : packoffset(c3);
    $endif
    $if USE_SHADOWMAP
      float4x4 LightProjectionTex[4] : packoffset(c5);
    $endif
    
  #else
  
    float4 LightPos;
    $if MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir;
    $endif
    $if USE_SHADOWMAP 
      float4x4 LightProjectionTex[4]; 
    $endif
    
  #endif
  
// Pixel shader
#else

  #if defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU) || defined(_VISION_PS3)
    float4 MaterialParams : register(c32);   
    $if USE_SHADOWMAP
      float4 ShadowParameters : register(c69);
    $endif
  #elif defined(_VISION_DX11)
    float4 MaterialParams : packoffset(c0);
    $if USE_SHADOWMAP
      float4 ShadowParameters : packoffset(c5);
    $endif
  #else
    float4 MaterialParams; // x:specular mul, y:specular exp, z=Parallax Scaling, w=Parallax Bias. Same as for Material itself
    $if USE_SHADOWMAP 
      float4 ShadowParameters;
    $endif
  #endif

#endif

//-----------------------------------------------------------------------------------
// Lighting specific shader constants

#if defined(_COMPUTES_LIGHTING)

  #if defined(_VISION_PSP2) || defined(_VISION_WIIU) || defined(_VISION_PS3)
  
    float4 LightRadius    : register(c34);
    float4 LightColor     : register(c33);
    
    $if !MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir     : register(c35);
    $endif
    $if SPOTLIGHT
      float4 LightAngle   : register(c36);
    $endif
    
  #elif defined(_VISION_GLES2)
    
    // using register >= 65 for vertex shader (GLES needs specific register binding)
    float4 LightRadius    : register(c65);
    float4 LightColor     : register(c66);
    
    $if !MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir     : register(c67);
    $endif
    $if SPOTLIGHT
      float4 LightAngle   : register(c68);
    $endif
    
  #elif defined(_VISION_DX11)
  
    float4 LightRadius    : packoffset(c1); // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
    float4 LightColor     : packoffset(c2); // xyz: color with pre-multiplied light multiplier
    
    $if !MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir     : packoffset(c3);
    $endif
    $if SPOTLIGHT
      float4 LightAngle   : packoffset(c4);
    $endif
    
  #else
    
    float4 LightRadius; // x:Radius, y:1/Radius (used for attenuation texture), z:Radius^2(not used), w:1/Radius^2(not used)
    float4 LightColor;  // xyz: color with pre-multiplied light multiplier
    
    $if !MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
      float3 LightDir;
    $endif
    $if SPOTLIGHT
      float4 LightAngle;
    $endif

  #endif

#endif

#if defined(_VISION_DX11)
  } // constant buffer end
#endif

//-----------------------------------------------------------------------------------

// Only include the follwing function definitions if lighting is computed 
// by the active shader type.
#if defined(_COMPUTES_LIGHTING)
  #include "DynamicLightingEquationsMobile.inc"
#endif

//-----------------------------------------------------------------------------------
// Vertex Shader Output

struct VS_OUT
{
  float4 vProjPos       : SV_Position;
  half3 UV0_FogCoord    : TEXCOORD0; // xy = UV0, z = fFogCoord
  
$if MOBILE_VERTEXLIGHTING
  half4 Color           : TEXCOORD1;
$else
  // light vec not needed for directional lights
  $if !DIRECTIONALLIGHT
    // in tangent space if using normal mapping,
    // in world space otherwise
    float3 vLightVec     : TEXCOORD1; 
  $endif
  
  // If we use normal mapping, we transform the light vec into tangent space,
  // so the normal is constant.
  $if !MOBILE_DYNAMICLIGHTNORMALMAPPING
    half3 vNormalWS     : TEXCOORD2;
  $elif DIRECTIONALLIGHT || SPOTLIGHT
    // light dir in tangent space
    half3 vLightDirTS   : TEXCOORD2;
  $endif
  
$endif
  
$if USE_SHADOWMAP
  float4 ShadowCoords    : TEXCOORD3;
  $if DIRECTIONALLIGHT
    float ViewDistance   : TEXCOORD4;
  $endif
$endif
};

//-----------------------------------------------------------------------------------
