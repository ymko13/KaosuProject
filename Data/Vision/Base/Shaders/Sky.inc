#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

#ifdef _VISION_DX11

cbuffer g_GlobalConstantBufferObject : register (b1)
{
  float4x4  matMVP  : packoffset(c4);   // model view projection matrix
}

#ifdef _IS_PIXEL_SHADER

cbuffer g_GlobalConstantBufferFrame : register (b0)
{
    float4    depthFogCol     : packoffset(c20);  // linear depth fog color
}

cbuffer g_GlobalConstantBufferUser : register (b2)
{
  float            FogCoord              : packoffset(c0);
}
#endif

cbuffer g_GlobalConstantBufferSky : register (b3)
{
  // Array with SKY_LAYER_COUNT entries: Modulation color for every layer
  float4   SkyColor[8]      : packoffset(c0);   

  // Array with pairs of 2 registers for every layer:
  //  reg[2n+0] : x:mapping type, y:sphere curvedness, z:sphere height offset
  //  reg[2n+1] : xy: texture tiling, zw: texture scroll ofs
  float4   SkyTransform[16] : packoffset(c8);
}

#else
#ifdef _IS_VERTEX_SHADER
  float4x4  matMVP  : register(c8);

  

  // Array with pairs of 2 registers for every layer:
  //  reg[2n+0] : x:mapping type, y:sphere curvedness, z:sphere height offset
  //  reg[2n+1] : xy: texture tiling, zw: texture scroll ofs
  float4   SkyTransform[$SKY_LAYER_COUNT*2] : register(c44);

#else
  // Array with SKY_LAYER_COUNT entries: Modulation color for every layer
  float4 SkyColor[$SKY_LAYER_COUNT] : register(c2);
  float4 depthFogCol : register(c1);  // linear depth fog color
  #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_GLES2) || defined(_VISION_WIIU)
    float             FogCoord              : register(c32);
  #else  
    float             FogCoord;
  #endif
#endif
#endif


/////////////////////////////////////////////////////////////////////////
// input structure; contains position and texture coordinates for cubemap
/////////////////////////////////////////////////////////////////////////
struct VS_IN
{
    float3 ObjPos   : POSITION;
    float3 UV0 : TEXCOORD0;  // xy=cubemap coordinates, z=horiz. cylinder angle
};


/////////////////////////////////////////////////////////////////////////
// output structure; contains texture coordinates for every sky layer
/////////////////////////////////////////////////////////////////////////
struct VS_OUT
{
  float4 ProjPos  : SV_Position;
  float2 UV0 : TEXCOORD0;
		
  $if SKY_LAYER_COUNT > 1
    float2 UV1 : TEXCOORD1;
  $endif
  $if SKY_LAYER_COUNT > 2
    float2 UV2 : TEXCOORD2;
  $endif
  $if SKY_LAYER_COUNT > 3
    float2 UV3 : TEXCOORD3;
  $endif
};

#ifdef _IS_VERTEX_SHADER

/////////////////////////////////////////////////////////////////////////
// Computes cubemap texture coordinates; not much to do here...
/////////////////////////////////////////////////////////////////////////
float2 ComputeCubemapCoords(VS_IN In)
{
  return In.UV0.xy;
}

/////////////////////////////////////////////////////////////////////////
// Computes cylinder coords here
/////////////////////////////////////////////////////////////////////////
float2 ComputeCylinderCoords(VS_IN In, float fHeightOfs)
{
  float3 pos = In.ObjPos;
  pos = normalize(pos);
  return float2(In.UV0.z, saturate(0.5f-pos.z+fHeightOfs)); // UV0.z is the angle [0..1] for 360deg
}


/////////////////////////////////////////////////////////////////////////
// Computes spherical texture coordinates from the 3D position
/////////////////////////////////////////////////////////////////////////
float2 ComputeSphericalCoords(VS_IN In, float4 transform, float2 params)
{
  float3 pos = In.ObjPos;
  pos.z *= params.x; // curvedness
  pos = normalize(pos);
  pos.z += params.y; // height ofs
  pos = normalize(pos);
  return pos.yx*transform.xy+transform.zw; // scale and scroll (swap xy for backwards compatibility)
}



/////////////////////////////////////////////////////////////////////////
// Computes texture coordinates dependent on the layer's mapping mode
//		mapping mode 0: spherical coordinates
//		mapping mode 1: cubemap coordinates
/////////////////////////////////////////////////////////////////////////

float2 GetTexCoordForLayer(VS_IN In, int iLayerStep)
{
// this seems to generate less instructions than the commented function below
  float fLayerType = SkyTransform[iLayerStep].x;
  if (fLayerType>1.5f)
    return ComputeCylinderCoords(In,SkyTransform[iLayerStep].z);
  else if (fLayerType>0.5f)
    return ComputeCubemapCoords(In);
  else
    return ComputeSphericalCoords(In,SkyTransform[iLayerStep+1],SkyTransform[iLayerStep].yz);
}

/*
float2 GetTexCoordForLayer(VS_IN In, int iLayerStep)
{
  float2 UVArray[2];
  UVArray[0] = ComputeSphericalCoords(In,SkyTransform[iLayerStep+1],SkyTransform[iLayerStep].yz);
  UVArray[1] = ComputeCubemapCoords(In);

  return UVArray[SkyTransform[iLayerStep].x];
}*/


/////////////////////////////////////////////////////////////////////////
// Vertex shader
/////////////////////////////////////////////////////////////////////////
VS_OUT vs_main( VS_IN In )
{
  VS_OUT Out;
  float3 scaledSkyPos = In.ObjPos;
  Out.ProjPos = mul( matMVP, float4(scaledSkyPos.xyz,1.0) );


  Out.UV0 = GetTexCoordForLayer(In,0);
  //Out.UV0 = In.UV0;
  
	$if SKY_LAYER_COUNT > 1
		Out.UV1 = GetTexCoordForLayer(In,2);
	$endif
	$if SKY_LAYER_COUNT > 2
		Out.UV2 = GetTexCoordForLayer(In,4);
	$endif
	$if SKY_LAYER_COUNT > 3
		Out.UV3 = GetTexCoordForLayer(In,6);
	$endif

  return Out;
}

#endif

#ifdef _IS_PIXEL_SHADER

#ifdef _VISION_DX11
  Texture2D <float4>  SkyTexture0         : register(t0);
  sampler             SkyTextureSampler0  : register(s0);
  Texture2D <float4>  SkyTexture1         : register(t1);
  sampler             SkyTextureSampler1  : register(s1);
  Texture2D <float4>  SkyTexture2         : register(t2);
  sampler             SkyTextureSampler2  : register(s2);
  Texture2D <float4>  SkyTexture3         : register(t3);
  sampler             SkyTextureSampler3  : register(s3);
#else
  sampler2D           SkyTexture0         : register(s0);
  sampler2D           SkyTexture1         : register(s1);
  sampler2D           SkyTexture2         : register(s2);
  sampler2D           SkyTexture3         : register(s3);
#endif

/////////////////////////////////////////////////////////////////////////
// Pixel shader
/////////////////////////////////////////////////////////////////////////
float4 ps_main( VS_OUT In ) : SV_Target
{
  float4 result = vTex2D(SkyTexture0, SkyTextureSampler0, In.UV0) * SkyColor[0];
  float4 next;
  
	$if SKY_LAYER_COUNT > 1
		next = vTex2D(SkyTexture1, SkyTextureSampler1, In.UV1) * SkyColor[1];
		result = lerp(result,next, next.a);
	$endif
	$if SKY_LAYER_COUNT > 2
		next = vTex2D(SkyTexture2, SkyTextureSampler2, In.UV2) * SkyColor[2];
		result = lerp(result,next, next.a);
	$endif
	$if SKY_LAYER_COUNT > 3
		next = vTex2D(SkyTexture3, SkyTextureSampler3, In.UV3) * SkyColor[3];
		result = lerp(result,next, next.a);
	$endif
  
  result.xyz = GetFogColor(result.xyz, depthFogCol.xyz, FogCoord);
  
  return result;
}

#endif