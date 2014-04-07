//
#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>

struct VS_OUT_BS
{
  float4 ProjPos  : SV_Position;
  float2 UV0      : TEXCOORD0;
  float3 Normal   : TEXCOORD1;
  float4 WorldPos : TEXCOORD2;
  float3 EyeVec   : TEXCOORD3;
  float3 Tangent  : TEXCOORD4;
  float3 Binormal : TEXCOORD5;
};

#ifdef _IS_VERTEX_SHADER

  #ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4 eyePos     : packoffset(c17);
  }
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMVP   : packoffset(c4);
    float4x4 matWorld : packoffset(c8);
  }
  #else
    float4x4 matMVP   : register(c8);
    float4x4 matWorld : register(c20);
    float4 eyePos     : register(c31);
  #endif
  
  struct VS_IN
  {
    float3 ObjPos  : POSITION;
    float3 Normal  : NORMAL;
    float2 UV0     : TEXCOORD0;
    float3 Tangent : TEXCOORD2;
  };
  
  VS_OUT_BS vs_main( VS_IN In )
  {
    VS_OUT_BS Out;
    Out.ProjPos = mul( matMVP, float4(In.ObjPos, 1.0f) );
    Out.UV0 = In.UV0;
    
    Out.Normal = mul((float3x3)matWorld, In.Normal);
    Out.WorldPos = mul(matWorld, float4(In.ObjPos, 1.0f));
    Out.EyeVec = eyePos - Out.WorldPos;
    
    float3 Normal = mul((float3x3)matWorld, In.Normal);
    float3 Tangent = mul((float3x3)matWorld, In.Tangent);
    
    float fMul = dot(Tangent, Tangent) - 2.0f;
    Tangent = normalize(Tangent);
    float3 biNormal = cross(Tangent, Normal)*fMul;
    
    Out.Tangent = Tangent;
    Out.Binormal = biNormal;
    
    return Out;
  }

#endif //_IS_VERTEX_SHADER


#ifdef _IS_PIXEL_SHADER

  #ifdef _VISION_DX11
    Texture2D BaseTexture        : register(t0);
    sampler   BaseTextureSampler : register(s0);
    Texture2D NormalMap          : register(t1);
    sampler   NormalMapSampler   : register(s1);
  #else
    sampler2D BaseTexture : register(s0);
    sampler2D NormalMap   : register(s1);
  #endif
  
  #ifdef _VISION_DX11
    cbuffer g_GlobalConstantBufferFrame : register (b0)
    {
      float4 cameraDirection : packoffset(c18);
    }
    cbuffer g_GlobalConstantBufferUserPS : register (b2)
    {
	  float4 Light0 : packoffset(c0);
	  float4 Light1 : packoffset(c1);
	  float4 Light2 : packoffset(c2);
    }
  #else
    float4 cameraDirection : register(c20);
	float4 Light0;
	float4 Light1;
	float4 Light2;
  #endif
    
  struct LightStruct  //struct with lightinfo, easier to work with.
  {
	float3 LightVec;
  	float3 LightColor;
  };
  
  void CreateLights(float3 worldSpacePos, out LightStruct lightsArray[3])
  {
    lightsArray[0].LightVec = Light0 - worldSpacePos;
    lightsArray[0].LightColor = float3(1.0f, 1.0f, 1.0f);
    
    lightsArray[1].LightVec = Light1 - worldSpacePos;
    lightsArray[1].LightColor = float3(0.371094f * 0.3f, 0.578125f * 0.3f, 0.808594f * 0.3f);
  	
    lightsArray[2].LightVec = Light2 - worldSpacePos;
    lightsArray[2].LightColor = float3(0.988281f * 0.3f, 0.531250f * 0.3f, 0.324219f * 0.3f);

  }
  
  float DiffuseLight(float3 normal, float3 lightvec)
  {
	normal = normalize(normal);
  	lightvec = normalize(lightvec);
		  
	return saturate(dot(normal, lightvec)); // dot product between surface and light returns how lit the pixel is. clamp between 0 and 1 because intensity is multiplied later
  }
  
  float3 BlinnSpecular(float3 normal, float3 lightvec, float3 eyevec, float glossiness)
  {
	normal = normalize(normal);
  	lightvec = normalize(lightvec);
	eyevec = normalize(eyevec);
  	
	float3 halfvector = normalize(eyevec + lightvec); // add eye and light together for half vector (Blinn)
  	
	float3 specular;
  	specular = dot(halfvector, normal); // dot between half and normal (Blinn)
	specular = pow(saturate(specular), glossiness); // power specular to glossiness to sharpen highlight
  	specular *= saturate(dot(normal, lightvec) * 4); // what this does is just make sure no specular happens on unlit parts. the multiplier works as a bias
	  
  	return specular;
  }
  
  float4 GetNormal(float2 UV)
  {
    return DecodeNormalAndAlpha(vTex2D(NormalMap, NormalMapSampler, UV));
  }
  
  float4 ps_main( VS_OUT_BS In ) : SV_Target
  {
    float4 color = vTex2D(BaseTexture, BaseTextureSampler, In.UV0);
#ifdef ALPHATEST
  clip(color.a - 0.01f);
#endif

    float3 specColor = float3(0.1f, 0.1f, 0.1f);
    
    float3 normal = GetWorldNormal(GetNormal(In.UV0), In.Tangent, In.Binormal, In.Normal);
    
    LightStruct lightsArray[3];
    CreateLights(In.WorldPos, lightsArray);
    
    float3 totalDiffuse =  float3(0.2f, 0.2f, 0.2f); //start off with ambient color
    float3 totalSpecular = float3(0.0f, 0.0f, 0.0f);
    
    for(int i = 0; i < 3; ++i) // for loop to iterate over our 3 lights
    {
      // diffuse term
      float diffuse;
      diffuse = DiffuseLight(normal, lightsArray[i].LightVec.xyz);
  
      // add every light to the total diffuse sum (lights are additive towards each other), multiplied with light color 
      totalDiffuse += (diffuse*lightsArray[i].LightColor);
  
      // specular term
      float3 specular = float3(1.0f, 1.0f, 1.0f);
      specular *= BlinnSpecular(normal, lightsArray[i].LightVec.xyz, In.EyeVec, 50.0f);
      specular *= specColor; // apply specular color
      specular *= lightsArray[i].LightColor; // multiply by light color
  
      totalSpecular += specular; // add every light specular to total specular level
    }
    
    float4 ret = color;
    ret = float4(pow(ret.x, 2.2f), pow(ret.y, 2.2f), pow(ret.z, 2.2f), ret.a);
    
    // multiply diffuse with color
    ret.rgb *= totalDiffuse;
    
    // ADD specular to colored diffuse
	ret.rgb += totalSpecular;
	
    ret.rgb = pow(ret.rgb, 1.0/2.2f);
    
    return ret;
  }

#endif //_IS_PIXEL_SHADER
