
#ifdef USE_SKINNING

  #ifdef _VISION_DX11

    cbuffer g_GlobalConstantBufferSkinning : register(b4)
    {  
      float4 _noExport_SkinMatrixArray[186] : packoffset(c0);
    }

  #else

    #if defined(_VISION_XENON)
      float4 _noExport_SkinMatrixArray[180] : register(c70);
    #elif defined(_VISION_PSP2)
      float4 _noExport_SkinMatrixArray[768] : register(c70);  // 3x4=12, times 256 bones max
    #else
      float4 _noExport_SkinMatrixArray[186] : register(c70);
    #endif

  #endif


  float3 TransformByArrayMatrix4(int iIndex, float3 vPos)
  {
    float3 fPos;
    fPos.x = dot(_noExport_SkinMatrixArray[iIndex], float4(vPos, 1.0f));
    fPos.y = dot(_noExport_SkinMatrixArray[iIndex+1], float4(vPos, 1.0f));
    fPos.z = dot(_noExport_SkinMatrixArray[iIndex+2], float4(vPos, 1.0f));
    return fPos;
  }

  float3 TransformByArrayMatrix3(int iIndex, float3 vPos)
  {
    float3 fPos;
    fPos.x = dot(_noExport_SkinMatrixArray[iIndex], float4(vPos, 0.0f));
    fPos.y = dot(_noExport_SkinMatrixArray[iIndex+1], float4(vPos, 0.0f));
    fPos.z = dot(_noExport_SkinMatrixArray[iIndex+2], float4(vPos, 0.0f));
    return fPos; 
  }

#endif

#define PREPARE_SKINNING(/*float4*/ BoneIndices) \
  int iIndex1 = BoneIndices.x; int iIndex2 = BoneIndices.y; int iIndex3 = BoneIndices.z; int iIndex4 = BoneIndices.w; \
  iIndex1 *= 3; iIndex2 *= 3; iIndex3 *= 3; iIndex4 *= 3; \

#if defined(_VISION_DX11) || defined(_VISION_DX9)

// these skinning macros optimize for cases with only one influencial bone
// might be used for other platforms as well, but performance should be profiled with MassScene sample

  #define TRANSFORM_OBJECT_POS(/*float4*/ ObjPosVariable, /*float3*/ InObjPos, /*float4*/ BoneWeights) \
    ObjPosVariable.xyz = TransformByArrayMatrix4(iIndex1, InObjPos) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex2, InObjPos) * BoneWeights.y; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex3, InObjPos) * BoneWeights.z; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex4, InObjPos) * BoneWeights.w; \
    } \
    ObjPosVariable.w = 1.0f; \

  #define TRANSFORM_OBJECT_NORMAL(/*float3*/ ObjNormalVariable, /*float3*/ InNormal, /*float4*/ BoneWeights) \
    ObjNormalVariable = TransformByArrayMatrix3(iIndex1, InNormal) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex2, InNormal) * BoneWeights.y; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex3, InNormal) * BoneWeights.z; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex4, InNormal) * BoneWeights.w; \
    }
    
  #define TRANSFORM_OBJECT_TANGENT(/*float3*/ ObjTangentVariable, /*float3*/ InTangent, /*float4*/ BoneWeights) \
    ObjTangentVariable = TransformByArrayMatrix3(iIndex1, InTangent) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex2, InTangent) * BoneWeights.y; \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex3, InTangent) * BoneWeights.z; \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex4, InTangent) * BoneWeights.w; \
    }

  #define SKIN(/*float4*/ ObjPosVariable, /*float3*/ObjNormalVariable, /*float3*/ObjTangentVariable, /*float3*/ InObjPos, /*float3*/ InNormal, /*float3*/ InTangent, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    ObjPosVariable.xyz = TransformByArrayMatrix4(iIndex1, InObjPos) * BoneWeights.x; \
    ObjNormalVariable = TransformByArrayMatrix3(iIndex1, InNormal) * BoneWeights.x; \
    ObjTangentVariable = TransformByArrayMatrix3(iIndex1, InTangent) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex2, InObjPos) * BoneWeights.y; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex2, InNormal) * BoneWeights.y; \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex2, InTangent) * BoneWeights.y; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex3, InObjPos) * BoneWeights.z; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex3, InNormal) * BoneWeights.z; \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex3, InTangent) * BoneWeights.z; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex4, InObjPos) * BoneWeights.w; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex4, InNormal) * BoneWeights.w; \
      ObjTangentVariable += TransformByArrayMatrix3(iIndex4, InTangent) * BoneWeights.w; \
    } \
    ObjPosVariable.w = 1.0f;
    
  #define SKIN_NO_TANGENT(/*float4*/ ObjPosVariable, /*float3*/ObjNormalVariable, /*float3*/ InObjPos, /*float3*/ InNormal, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    ObjPosVariable.xyz = TransformByArrayMatrix4(iIndex1, InObjPos) * BoneWeights.x; \
    ObjNormalVariable = TransformByArrayMatrix3(iIndex1, InNormal) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex2, InObjPos) * BoneWeights.y; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex2, InNormal) * BoneWeights.y; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex3, InObjPos) * BoneWeights.z; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex3, InNormal) * BoneWeights.z; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex4, InObjPos) * BoneWeights.w; \
      ObjNormalVariable += TransformByArrayMatrix3(iIndex4, InNormal) * BoneWeights.w; \
    } \
    ObjPosVariable.w = 1.0f;

    
  #define SKIN_NO_NORMAL(/*float4*/ ObjPosVariable, /*float3*/ InObjPos, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    ObjPosVariable.xyz = TransformByArrayMatrix4(iIndex1, InObjPos) * BoneWeights.x; \
    if (BoneWeights.y>0.0f) \
    { \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex2, InObjPos) * BoneWeights.y; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex3, InObjPos) * BoneWeights.z; \
      ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex4, InObjPos) * BoneWeights.w; \
    } \
    ObjPosVariable.w = 1.0f;


#else // DX9 || DX11

  #define TRANSFORM_OBJECT_POS(/*float4*/ ObjPosVariable, /*float3*/ InObjPos, /*float4*/ BoneWeights) \
    ObjPosVariable.xyz = TransformByArrayMatrix4(iIndex1, InObjPos) * BoneWeights.x; \
    ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex2, InObjPos) * BoneWeights.y; \
    ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex3, InObjPos) * BoneWeights.z; \
    ObjPosVariable.xyz += TransformByArrayMatrix4(iIndex4, InObjPos) * BoneWeights.w; \
    ObjPosVariable.w = 1.0f; \

  #define TRANSFORM_OBJECT_NORMAL(/*float3*/ ObjNormalVariable, /*float3*/ InNormal, /*float4*/ BoneWeights) \
    ObjNormalVariable = TransformByArrayMatrix3(iIndex1, InNormal) * BoneWeights.x; \
    ObjNormalVariable += TransformByArrayMatrix3(iIndex2, InNormal) * BoneWeights.y; \
    ObjNormalVariable += TransformByArrayMatrix3(iIndex3, InNormal) * BoneWeights.z; \
    ObjNormalVariable += TransformByArrayMatrix3(iIndex4, InNormal) * BoneWeights.w; \
    
  #define TRANSFORM_OBJECT_TANGENT(/*float3*/ ObjTangentVariable, /*float3*/ InTangent, /*float4*/ BoneWeights) \
    ObjTangentVariable = TransformByArrayMatrix3(iIndex1, InTangent) * BoneWeights.x; \
    ObjTangentVariable += TransformByArrayMatrix3(iIndex2, InTangent) * BoneWeights.y; \
    ObjTangentVariable += TransformByArrayMatrix3(iIndex3, InTangent) * BoneWeights.z; \
    ObjTangentVariable += TransformByArrayMatrix3(iIndex4, InTangent) * BoneWeights.w; \
    
  #define SKIN(/*float4*/ ObjPosVariable, /*float3*/ObjNormalVariable, /*float3*/ObjTangentVariable, /*float3*/ InObjPos, /*float3*/ InNormal, /*float3*/ InTangent, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    TRANSFORM_OBJECT_POS(ObjPosVariable, InObjPos, BoneWeights) \
    TRANSFORM_OBJECT_NORMAL(ObjNormalVariable, InNormal, BoneWeights) \
    TRANSFORM_OBJECT_TANGENT(ObjTangentVariable, InTangent, BoneWeights) \

  #define SKIN_NO_TANGENT(/*float4*/ ObjPosVariable, /*float3*/ObjNormalVariable, /*float3*/ InObjPos, /*float3*/ InNormal, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    TRANSFORM_OBJECT_POS(ObjPosVariable, InObjPos, BoneWeights) \
    TRANSFORM_OBJECT_NORMAL(ObjNormalVariable, InNormal, BoneWeights) \
    
  #define SKIN_NO_NORMAL(/*float4*/ ObjPosVariable, /*float3*/ InObjPos, /*float4*/BoneIndices, /*float4*/BoneWeights) \
    PREPARE_SKINNING(BoneIndices) \
    TRANSFORM_OBJECT_POS(ObjPosVariable, InObjPos, BoneWeights) \

#endif // DX9 || DX11
