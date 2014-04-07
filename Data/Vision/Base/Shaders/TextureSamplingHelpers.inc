
#ifndef VISION_TEXTURE_SAMPLING_HELPERS
#define VISION_TEXTURE_SAMPLING_HELPERS

// (manual) bilinear filtering
#ifdef _VISION_DX11
  float4 SampleBilinear(Texture2D tex, sampler textureSampler, float2 fPixelCoords, float2 fInvTexSize)
#else
  #define SampleBilinear(_texture, _textureSampler, _fPixelCoords, _fInvTexSize) _SampleBilinear(_texture, _fPixelCoords, _fInvTexSize)
  float4 _SampleBilinear(sampler2D tex, float2 fPixelCoords, float2 fInvTexSize)
#endif
{
#if defined(_VISION_XENON) || defined(_VISION_DX9)
  float2 tmp = fPixelCoords - 0.5f;
  float2 fractionalPart = frac(tmp); // frac is a direct GPU instruction, so it is faster than modf or floor (which both use frac)
  float2 intPart = tmp - fractionalPart + 0.5f;
#else
  float2 fractionalPart = frac(fPixelCoords);
  float2 intPart = fPixelCoords - fractionalPart;
#endif

  float2 lowerTexcoord = intPart * fInvTexSize;
  float2 upperTexcoord = lowerTexcoord + fInvTexSize;

	float4 colorLL = vTex2D(tex, textureSampler, lowerTexcoord);
	float4 colorLR = vTex2D(tex, textureSampler, float2(upperTexcoord.x, lowerTexcoord.y));
	float4 colorUL = vTex2D(tex, textureSampler, float2(lowerTexcoord.x, upperTexcoord.y));
	float4 colorUR = vTex2D(tex, textureSampler, upperTexcoord);

	return lerp(lerp(colorLL, colorLR, fractionalPart.x), lerp(colorUL, colorUR, fractionalPart.x), fractionalPart.y);
}

#endif
