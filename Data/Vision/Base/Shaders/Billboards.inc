#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

#ifdef USE_DEFERRED
  #include <Shaders/DeferredShadingHelpers.inc>
#endif 


#ifdef _VISION_DX11
cbuffer g_GlobalConstantBufferFrame : register (b0)
{
  float4x4 matEye        : packoffset(c4);   // eye matrix
  float4x4 matInvEye     : packoffset(c8);   // inverse eye matrix
  float4 contextCP       : packoffset(c12);  // context clip planes: near, far, 1/(far-near), w=LODScale
  float4 cameraPos       : packoffset(c17);  // camera position in world space
  float4 depthFog        : packoffset(c19);  // linear depth fog parameter: near, far, 1/(far-near)
  float4 depthFogCol     : packoffset(c20);  // linear depth fog color
  float4 gAmbientCol     : packoffset(c21);  // global ambient color
}
cbuffer g_GlobalConstantBufferObject : register (b1)
{
  float4x4 matMV   : packoffset(c0);   // model view matrix
  float4x4 matMVP  : packoffset(c4);   // model view projection matrix
  float4x4 matM2WS : packoffset(c8);  // model to worldspace
}
cbuffer g_GlobalConstantBufferUser : register (b2)
{
  float2   ClipDistances;   // x=near, y=far
}

#else
  // VS:
  float4x4 matMV     : register(c0);
  float4x4 matMVP    : register(c8);
  float4x4 matInvEye : register(c16);
  float4x4 matM2WS   : register(c20);
  float4   contextCP : register(c24);
  float4 cameraPos   : register(c31);
  float4 depthFog    : register(c62);  // linear depth fog parameter: near, far, 1/(far-near)
  float2 ClipDistances : register(c64);   // x=near, y=far

  // PS:
  float4 depthFogCol : register(c1);   // linear depth fog color
  float3 gAmbientCol : register(c21);
#endif


////////////////////////////////////////////////////////////////
// Vertex Decl
////////////////////////////////////////////////////////////////


struct VS_IN
{
  float3   ObjPos  : POSITION;
  float4   Color   : COLOR;
  float2   UV0     : TEXCOORD0;
#ifndef BILLBOARDS_AS_CROSSES
  float2   Stretch : TEXCOORD1;
#endif
};


struct VS_OUT
{
  float4   ProjPos : SV_Position;
#ifdef USE_FOG
  float3   UV0AndFog : TEXCOORD0;
#else
  float2   UV0AndFog : TEXCOORD0;
#endif
  float4   Color   : TEXCOORD1;
#ifdef USE_DEFERRED
  float4   NormalAndDepth   : TEXCOORD2;
#endif
};

////////////////////////////////////////////////////////////////
// Vertex shader
////////////////////////////////////////////////////////////////

VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
  float4 vDepth = mul( matM2WS, float4(In.ObjPos.xyz, 1.0f) ) - cameraPos;
#ifdef BILLBOARDS_AS_CROSSES
  float3 wsPos = In.ObjPos.xyz; // standard geometry
#else
  #ifdef BILLBOARD_ALWAYS_UP
  //  float3 vDepth = matInvEye._m02_m12_m23;
    float3 vScreenSpanV = float3(0,0,1); // up vector
    float3 vScreenSpanU = normalize(cross(vScreenSpanV,vDepth.xyz));
  #else
    float3 vScreenSpanU = matInvEye._m00_m10_m20;
    float3 vScreenSpanV = matInvEye._m01_m11_m21;
  #endif
  float3 wsPos = In.ObjPos.xyz;
#ifdef USE_CLIPDISTANCES
  float fLODScale = contextCP.w;
  float fDist = length(vDepth.xyz) * fLODScale;
  if (fDist>=ClipDistances.x && fDist<=ClipDistances.y) // when clipping this billboard is collapsed...
#endif
  wsPos += vScreenSpanU.xyz*In.Stretch.x + vScreenSpanV.xyz*In.Stretch.y;
#endif
  Out.ProjPos = mul( matMVP, float4(wsPos, 1.0f) );
  Out.UV0AndFog.xy = In.UV0;
#ifdef USE_FOG
  Out.UV0AndFog.z = GetFogCoord(depthFog, dot(matMV._m20_m21_m22_m23, float4(wsPos, 1.0f))); // used to be Out.ProjPos.z but that messes with the oblique clipping plane of mirrors
#endif
  Out.Color = In.Color;
#ifdef USE_DEFERRED
  #ifdef BILLBOARDS_AS_CROSSES
    Out.NormalAndDepth.xyz = matInvEye._m02_m12_m22; // normals in mesh?
  #else
    #ifdef BILLBOARD_ALWAYS_UP
      Out.NormalAndDepth.xyz = float3(0,0,1); // in this mode, pick up lighting from straight above
    #else
      Out.NormalAndDepth.xyz = cross(vScreenSpanV,vScreenSpanU);
    #endif
  #endif
  Out.NormalAndDepth.w = mul(matMV, float4(wsPos, 1.0f)).z / contextCP.y;
#endif
 
  return Out;
}


////////////////////////////////////////////////////////////////
// Pixel shader
////////////////////////////////////////////////////////////////

#ifdef _VISION_DX11
  Texture2D <float4> BaseTexture        : register(t0);
  sampler            BaseTextureSampler : register(s0);
#else
  sampler2D          BaseTexture        : register(s0);
#endif


#ifdef USE_DEFERRED

// deferred version
PS_OUT ps_main( VS_OUT In )
{
  PixelDataUnpacked pixelData;

  float4 result = vTex2D(BaseTexture, BaseTextureSampler, In.UV0AndFog.xy) * In.Color;
#ifdef ALPHATEST
  clip(result.a - 0.25);
#endif

  pixelData.vDiffColor = result.xyz;
  pixelData.vNormal = In.NormalAndDepth.xyz;
  pixelData.fSpecExp = 1.0f;
  pixelData.vSpecColor = float3(0.0f, 0.0f, 0.0f);
  pixelData.vAccInitial = gAmbientCol*result.xyz;
  pixelData.fDepth = In.NormalAndDepth.w;  
  pixelData.fMaterial = 1.0f;

  return packPixelValues(pixelData);  
}

#else

// non-deferred version
float4 ps_main( VS_OUT In ) : SV_Target
{
  float4 result = vTex2D(BaseTexture, BaseTextureSampler, In.UV0AndFog.xy) * In.Color;
#ifdef ALPHATEST
  clip(result.a - 0.25);
#endif

#ifdef USE_FOG
  result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, In.UV0AndFog.z);  
#endif

  return result;
}

#endif
