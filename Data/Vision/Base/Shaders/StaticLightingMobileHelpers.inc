#include <shaders/VisionCommon.inc>
#include <shaders/ShaderHelpers.inc>

$if VMATERIAL_BUMP
  #define USE_TANGENTSPACE_VS
  #define USE_NORMALMAP
  $redefine USE_NORMAL_VS = true
  $redefine USE_TANGENT_VS = true
$endif

$if VMATERIAL_BUMPANIMATED
  #if !defined(USE_TANGENTSPACE_VS)
    #define USE_TANGENTSPACE_VS
  #endif
  
  #if !defined(USE_NORMALMAP)
    #define USE_NORMALMAP
  #endif  
    
  $redefine USE_NORMAL_VS = true
  $redefine USE_TANGENT_VS = true
$endif

$if VMATERIAL_CUBEMAP
  #define USE_CUBEMAP
  #define USE_NORMALS_PS
  $redefine USE_NORMAL_VS = true
$endif

$if VMATERIAL_BUMPCUBEMAP
  #if !defined(USE_CUBEMAP)
    #define USE_CUBEMAP
  #endif
  
  #if !defined(USE_NORMALMAP)
    #define USE_NORMALMAP
  #endif
	
  #define USE_TANGENTSPACE_PS
  $redefine USE_NORMAL_VS = true
  $redefine USE_TANGENT_VS = true
$endif

$if VMATERIAL_FAKESPECULAR
  #define USE_CUBEMAP
  #define USE_NORMALMAP
  #define USE_TANGENTSPACE_PS
  
  $if defined(VMATERIAL_REFMAP)
    #define USE_SPECMAP
  $endif
$endif

$if LIGHTGRIDSIMPLE
  #if !defined(USE_NORMALS_PS)
	#define USE_NORMALS_PS
  #endif
  $redefine USE_NORMAL_VS = true
$endif

$if MOBILE_DYNAMICBASEPASSLIGHTING 

  #if !defined(USE_NORMALS_VS)
    #define USE_NORMALS_VS
  #endif
  
  $if MOBILE_DYNAMICLIGHTNORMALMAPPING
    #if !defined(USE_TANGENTSPACE_VS)
      #define USE_TANGENTSPACE_VS
    #endif
    
    #if !defined(USE_NORMALMAP)
      #define USE_NORMALMAP
    #endif
   
    $if DIRECTIONALLIGHT || SPOTLIGHT
      #if !defined(USE_NORMALS_PS)
        #define USE_NORMALS_PS
      #endif
      #if !defined(USE_TANGENTSPACE_PS)
        #define USE_TANGENTSPACE_PS
      #endif
    $endif
  $endif
  
  $if !MOBILE_VERTEXLIGHTING && !MOBILE_DYNAMICLIGHTNORMALMAPPING
    #if !defined(USE_NORMALS_PS)
      #define USE_NORMALS_PS
    #endif
  $endif
  
  $if !DIRECTIONALLIGHT
    #if !defined(CALCULATE_POSITIONWS)
      #define CALCULATE_POSITIONWS
    #endif
  $endif
    
$endif

$if VMATERIAL_CUBEMAP || VMATERIAL_BUMPCUBEMAP || VMATERIAL_FAKESPECULAR || USE_SHADOWMAP 
  #if !defined(CALCULATE_POSITIONWS)
    #define CALCULATE_POSITIONWS
  #endif
$endif

$if VMATERIAL_CUBEMAP || VMATERIAL_BUMPCUBEMAP || VMATERIAL_FAKESPECULAR || (USE_SHADOWMAP && DIRECTIONALLIGHT)
  #if !defined(USE_EYEDIRWS)
    #define USE_EYEDIRWS
  #endif
$endif

#if defined(USE_TANGENTSPACE_PS) && !defined(USE_NORMALS_PS)
  #define USE_NORMALS_PS
#endif

#if defined(USE_TANGENTSPACE_PS) && !defined(USE_TANGENTSPACE_VS)
  #define USE_TANGENTSPACE_VS
#endif

#if defined(USE_NORMALS_PS) && !defined(USE_NORMALS_VS)
  #define USE_NORMALS_VS
#endif

#if defined(USE_TANGENTSPACE_VS) && !defined(USE_NORMALS_VS)
  #define USE_NORMALS_VS
#endif

$if !NOFOG || VMATERIAL_BUMP || LIGHTGRIDSIMPLE
  #if !defined(USE_LIGHTDIR_FOGCOORD)
    #define USE_LIGHTDIR_FOGCOORD
  #endif
$endif

$if VMATERIAL_BUMP || (MOBILE_DYNAMICBASEPASSLIGHTING && MOBILE_DYNAMICLIGHTNORMALMAPPING && !DIRECTIONALLIGHT)
  #if !defined(CALCULATE_BINORMAL_VS)
    #define CALCULATE_BINORMAL_VS
  #endif
$endif

#if defined(USE_NORMALS_PS) && defined(USE_TANGENTSPACE_PS) && defined(USE_NORMALMAP)
  #if !defined(CALCULATE_BINORMAL_PS)
    #define CALCULATE_BINORMAL_PS
  #endif
#endif

$if MOBILE_DYNAMICBASEPASSLIGHTING && MOBILE_DYNAMICLIGHTNORMALMAPPING && (DIRECTIONALLIGHT || SPOTLIGHT)
  #if !defined(CALCULATE_BINORMAL_PS)
    #define CALCULATE_BINORMAL_PS
  #endif
$endif

$if USE_SHADOWMAP && MOBILE_SUBTRACTIVE_SHADOWMAP
  #ifndef USE_NORMALS_VS
    #define USE_NORMALS_VS
  #endif
$endif

#if defined(_VISION_GLES2)
  // Please note: on all relevant devices (iOS, Android PowerVR/ Adreno) the performance was equal or better with half than with fixed
  #define FIXED half
  #define FIXED2 half2
  #define FIXED3 half3
  #define FIXED4 half4
#else
  #define FIXED half
  #define FIXED2 half2
  #define FIXED3 half3
  #define FIXED4 half4
#endif

struct PS_IN
{
  float4 ProjPos  : SV_Position;
  half4 UV0 : TEXCOORD0;  //diffuse & normalmap 
  half4 UV1 : TEXCOORD1;  //lightmap
 
  #ifdef USE_EYEDIRWS
    float4 EyeDirWS : TEXCOORD2; 
  #endif
  
  #ifdef USE_LIGHTDIR_FOGCOORD
    half4 LightDirFogCoord : TEXCOORD3; // xyz = LightDir, w = fogCoord
  #endif
  
  $if MOBILE_DYNAMICBASEPASSLIGHTING
    $if MOBILE_VERTEXLIGHTING
      half4 VertexColor: TEXCOORD4;
    $elif !DIRECTIONALLIGHT
      float3 vLightVec: TEXCOORD4; // in tangent space if using normal mapping in world space otherwise
    $endif
  $endif  
  
  #if defined(USE_NORMALS_PS)
    half3 NormalWS : TEXCOORD5;
  #endif
  
  #if defined(USE_TANGENTSPACE_PS)  
    half4 TangentWS : TEXCOORD6;
  #endif
  
  $if USE_SHADOWMAP
    float4 ShadowCoords: TEXCOORD7;
  $endif
};