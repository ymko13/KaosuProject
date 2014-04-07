
struct PS_OUT
{
  float4 Accumulation : SV_Target0;
  float4 Diffuse : SV_Target1;
  float4 Normal : SV_Target2;
  float4 DepthSpec : SV_Target3;
};

struct PixelDataPacked
{
  float4 Accumulation;
  float4 Diffuse;
  float4 Normal;
  float4 DepthSpec;
};

struct PixelDataUnpacked
{
  float3 vAccInitial;
  float fDepth;
  float3 vNormal;
  float3 vDiffColor;
  float3 vSpecColor;
  float fSpecExp;
  float fMaterial;
};

#ifdef _VISION_XENON

float3 expandFloatToFloat3_16Bits(float compressedFloat)
{ 
  float ip;
  float3 uncompressedComponents;
  uncompressedComponents.b = modf(compressedFloat * 0.125f, ip);
  uncompressedComponents.g = modf(ip * 0.125f, ip);
  uncompressedComponents.r = ip * 0.125f;
  return uncompressedComponents;
} 

float compressFloat3ToFloat_16Bits(float3 originalValue)
{
  float3 truncated = trunc(originalValue * 7.0f);
  return truncated.r * 64.f + truncated.g * 8.f + truncated.b;
}

#else

float3 expandFloatToFloat3_16Bits(float compressedFloat)
{ 
  float ip;
  float3 uncompressedComponents;
  uncompressedComponents.b = modf(compressedFloat * 0.0625f, ip);
  uncompressedComponents.g = modf(ip * 0.0625f, ip);
  uncompressedComponents.r = ip * 0.0625f;
  return uncompressedComponents;
} 

float compressFloat3ToFloat_16Bits(float3 originalValue)
{
  float3 truncated = trunc(originalValue * 15.0f);
  return truncated.r * 256.f + truncated.g * 16.f + truncated.b;
}

#endif

inline float2 packNormalizedFloatInTwoBytes(float inVal)
{
  float2 outVal;
  float integerPart;
  outVal.y = modf(inVal * 256.f, integerPart);
  outVal.x = integerPart / 256.f;
  return outVal;
}

inline float unpackNormalizedFloatFromTwoBytes(float2 inVal)
{
  return inVal.x + inVal.y / 256.f;
}


inline PS_OUT packPixelValues(PixelDataUnpacked pixelData)
{
  PS_OUT Out;
  Out.Accumulation = float4(pixelData.vAccInitial, 1.0f);
  Out.Diffuse = float4(pixelData.vDiffColor, pixelData.fSpecExp);
  Out.Normal = float4(pixelData.vNormal*0.5f + 0.5f, pixelData.fMaterial);
  #ifdef _VISION_PS3
    float temp = pack_2half(half2(pixelData.fDepth, compressFloat3ToFloat_16Bits(pixelData.vSpecColor)));
    Out.DepthSpec = unpack_4ubyte(temp);
  #else
    #ifdef SPECULAR_COLOR_MONO
      Out.DepthSpec = float4(pixelData.fDepth, (pixelData.vSpecColor.x + pixelData.vSpecColor.y + pixelData.vSpecColor.z) * 0.33333f, 0.0f, 0.0f);
    #else    
      Out.DepthSpec = float4(pixelData.fDepth, compressFloat3ToFloat_16Bits(pixelData.vSpecColor), 0.0f, 0.0f);
    #endif
  #endif
  return Out;
}


inline PixelDataUnpacked unpackPixelValues(PixelDataPacked packedPixelData)
{
  PixelDataUnpacked unpackedPixel;
  unpackedPixel.vAccInitial = packedPixelData.Accumulation.xyz;
  unpackedPixel.vNormal = (packedPixelData.Normal.xyz*2.0f) - 1.0f;
  unpackedPixel.fMaterial = packedPixelData.Normal.w;
  unpackedPixel.vDiffColor = packedPixelData.Diffuse.xyz;
  #ifdef _VISION_PS3
    half2 halfTemp = unpack_2half(pack_4ubyte(packedPixelData.DepthSpec));
    unpackedPixel.fDepth = halfTemp.x;
    unpackedPixel.vSpecColor.xyz = expandFloatToFloat3_16Bits(halfTemp.y);
  #else
    unpackedPixel.fDepth = packedPixelData.DepthSpec.x;
    #ifdef SPECULAR_COLOR_MONO
      unpackedPixel.vSpecColor = packedPixelData.DepthSpec.yyy;
    #else
      unpackedPixel.vSpecColor.xyz = expandFloatToFloat3_16Bits(packedPixelData.DepthSpec.y);
    #endif
  #endif
  unpackedPixel.fSpecExp = packedPixelData.Diffuse.w;
  return unpackedPixel;
}

#define TRANSFORM_VIEW_DIRECTION(_invEyeMatrix, _inDir) mul(_invEyeMatrix, float4(_inDir.xyz, 0.0f))

#if defined(_VISION_PS3)

  #define COMPUTE_CONVERTED_DEPTH(_inVal) unpack_4ubyte(pack_2half(half2(_inVal,0.0f)))
  #define READ_CONVERTED_DEPTH(_buffer, _sampler, _uv) unpack_2half(pack_4ubyte(vTex2D(_buffer, _sampler, _uv))).x
  #define READ_DEPTH_BLUR(_buffer, _sampler, _uv) unpack_2half(pack_4ubyte(vTex2D(_buffer, _sampler, _uv)))
  
  #define READ_FROM_D24(_buffer, _uv) tex2D( _buffer,  _uv ).r
  
#elif defined(_VISION_PSP2)

  #define COMPUTE_CONVERTED_DEPTH(_inVal) _inVal
  #define READ_CONVERTED_DEPTH(_buffer, _sampler, _uv) tex2D<float>(_buffer, _uv)
  #define READ_DEPTH_BLUR(_buffer, _sampler, _uv) tex2D<float2>(_buffer, _uv)
  #define READ_FROM_D24(_buffer, _uv) tex2D<float>( _buffer,  _uv )
  
#elif defined(_VISION_WIIU)

  #define COMPUTE_CONVERTED_DEPTH(_inVal) _inVal
  #define READ_CONVERTED_DEPTH(_buffer, _sampler, _uv) vTex2D(_buffer, _sampler, _uv).r
  #define READ_DEPTH_BLUR(_buffer, _sampler, _uv) vTex2D( _buffer, _sampler, _uv )
  #define READ_FROM_D24(_buffer, _uv) tex2D( _buffer,  _uv ).r

#else

  #define COMPUTE_CONVERTED_DEPTH(_inVal) _inVal
  #define READ_CONVERTED_DEPTH(_buffer, _sampler, _uv) vTex2D(_buffer, _sampler, _uv).r
  #define READ_DEPTH_BLUR(_buffer, _sampler, _uv) vTex2D( _buffer, _sampler, _uv )
  #define READ_FROM_D24(_buffer, _uv) tex2D( _buffer,  _uv ).r

#endif    

inline float3 positionForDepth(float fDepth, float3 vEyePos, float3 vViewDir)
{
  float3 vRes = vEyePos + fDepth * vViewDir;
  return vRes;
}

#define mapToNormalizedRangeFrom255(VAL) (VAL*0.00390625f)

