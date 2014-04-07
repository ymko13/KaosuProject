//
#include <Shaders/VisionCommon.inc>

#define SKIP_DEFAULT_CONSTANT_BUFFER

#ifdef USE_TERRAIN
  #include "TerrainShaders.inc"
#endif

struct VS_OUT_BS
{
  float4 ProjPos  : SV_Position;
  float3 UV : TEXCOORD0; // z=normal
};

#ifdef _IS_VERTEX_SHADER

  #ifndef USE_TERRAIN
    #ifdef _VISION_DX11
      cbuffer g_GlobalConstantBufferFrame : register (b0)
      {
        float4x4 matPJ : packoffset(c0);   // projection matrix
        float4x4 matEye : packoffset(c4);  // eye matrix
      }

      cbuffer g_GlobalConstantBufferObject : register (b1)
      {
        float4x4 matMVP : packoffset(c4);   // model view projection matrix
        float4x4 matM2WS : packoffset(c8);  // model to world space matrix
      }
    #else
      float4x4 matPJ : register(c4);
      float4x4 matMVP : register(c8);
      float4x4 matEye : register(c12);
      float4x4 matM2WS : register(c20);
    #endif
  #endif //USE_TERRAIN

  #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
    float4 TexRefPlaneU : register(c64);
    float4 TexRefPlaneV : register(c65);
  #elif defined(_VISION_DX11)
    cbuffer g_GlobalConstantBufferUserVS : register (b2)
    {
      float4 TexRefPlaneU : packoffset(c0);
      float4 TexRefPlaneV : packoffset(c1);
    }
  #else
    float4 TexRefPlaneU;
    float4 TexRefPlaneV;
  #endif

  #ifndef USE_TERRAIN
    struct VS_IN
    {
      float3 ObjPos   : POSITION;
      float3 Normal   : NORMAL;
    };
  #endif

  #ifndef USE_TERRAIN
    float4 GetVertexPosition(VS_IN In)
    {
      float4 wsPos = mul( matM2WS, float4(In.ObjPos.xyz,1.0) );
      return wsPos;
    }
  #endif

  VS_OUT_BS vs_main_BS( VS_IN In )
  {
    VS_OUT_BS Out;
    float4 wsPos = GetVertexPosition(In);
    Out.UV.x = dot(TexRefPlaneU,wsPos);
    Out.UV.y = dot(TexRefPlaneV,wsPos);
    
    #ifdef USE_TERRAIN
      Out.ProjPos = mul( matMVP, wsPos );
      Out.UV.z = 1.0; // simplified
    #else
      Out.ProjPos = mul( matMVP, float4(In.ObjPos.xyz,1.0) );
      Out.UV.z = In.Normal.z;
    #endif
    
    return Out;
  }

#endif //_IS_VERTEX_SHADER


#ifdef _IS_PIXEL_SHADER

  #ifdef _VISION_DX11
    Texture2D <float4> ShadowTexture         : register(t0);
    sampler            ShadowTextureSampler  : register(s0);
  #else
    sampler2D          ShadowTexture         : register(s0);
  #endif

  #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
    float4 ShadowColor : register(c32);
  #elif defined(_VISION_DX11)

    cbuffer g_GlobalConstantBufferUserPS : register (b2)
    {
      float4 ShadowColor : packoffset(c0);
    }

  #else
    float4 ShadowColor;
  #endif




  float4 ps_main_BS( VS_OUT_BS In ) : SV_Target
  {
    float fGradient = In.UV.z;
    
    #ifndef _VISION_GLES2
      clip(fGradient-0.001);
    #endif
    
    float4 color = vTex2D(ShadowTexture, ShadowTextureSampler, In.UV.xy);
    color.rgb *= fGradient;
    return color*ShadowColor;
  }

#endif //_IS_PIXEL_SHADER
