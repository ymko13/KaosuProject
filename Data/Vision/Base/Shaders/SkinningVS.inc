
#include <shaders/SkinningSharedVS.inc>

#define SKINNING_VERTEX_STREAMS

#ifdef _VISION_DX11

  #ifdef USE_SKINNING
    #undef SKINNING_VERTEX_STREAMS
    #define SKINNING_VERTEX_STREAMS \
    int4 BoneIndices  : TEXCOORD3; \
    float4 BoneWeight : TEXCOORD4;
  #endif

#else
  
  #ifdef USE_SKINNING
    #undef SKINNING_VERTEX_STREAMS
    #define SKINNING_VERTEX_STREAMS \
    float4 BoneIndices : TEXCOORD3; \
    float4 BoneWeight  : TEXCOORD4;
  #endif
 
#endif
