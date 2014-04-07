// Make writing platform independent shader easier
#ifndef _VISION_COMMON_
    #define _VISION_COMMON_

    #ifdef _VISION_DX11
     
      #if defined(_DX9_ON_DX11) && defined(_USE_DOWNLEVEL_DECLARATIONS)
      
        #define vTex2D(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D1Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D2Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D3Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D4Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2Dproj(_vtex_, _vsampl_, _vcoord_) tex2Dproj(_vtex_, _vcoord_)
        #define vTex2Dlod(_vtex_, _vsampl_, _vcoord_) tex2Dlod(_vtex_, _vcoord_)
        #define vTex2Dgrad(_vtex_, _vsampl_, _vcoord_, _ddx_, _ddy_) tex2Dgrad(_vtex_, _vcoord_, _ddx_, _ddy_)
        #define vTex3D(_vtex_, _vsampl_, _vcoord_) tex3D(_vtex_, _vcoord_)
        #define vTexCUBE(_vtex_, _vsampl_, _vcoord_) texCUBE(_vtex_, _vcoord_)
        #define vTexCUBEbias(_vtex_, _vsampl_, _vcoord_) texCUBEbias(_vtex_, _vcoord_)
        #define vTexCUBElod(_vtex_, _vsampl_, _vcoord_) texCUBElod(_vtex_, _vcoord_)
        #define vLoadTex2D(_vtex_, _vcoord_) tex2D(_vtex_, _vcoord_)

        #define DECLARE_TEX2D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          sampler2D _texturename_ : register(s##_samplerregister_);
           
        #define DECLARE_TEX2D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          sampler2D _texturename_ : register(s##_samplerregister_);

        #define DECLARE_TEXCUBE_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          samplerCUBE _texturename_ : register(s##_samplerregister_);
            
        #define DECLARE_TEXCUBE_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          samplerCUBE _texturename_ : register(s##_samplerregister_);

        #define DECLARE_TEX3D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          sampler3D _texturename_ : register(s##_samplerregister_);
            
        #define DECLARE_TEX3D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          sampler3D _texturename_ : register(s##_samplerregister_);
          
      #else
  
        #define VFACING SV_IsFrontFace
        #define VFACING_TYPE bool
        #define vTex2D(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTex2D1Comp(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTex2D2Comp(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTex2D3Comp(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTex2D4Comp(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTex2Dproj(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_/_vcoord_.w)
        #define vTex2Dlod(_vtex_, _vsampl_, _vcoord_) _vtex_.SampleLevel(_vsampl_, _vcoord_, _vcoord_.z)
        #define vTex2Dgrad(_vtex_, _vsampl_, _vcoord_, _ddx_, _ddy_) _vtex_.SampleGrad(_vsampl_, _vcoord_, _ddx_, _ddy_)
        #define vTex3D(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTexCUBE(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_)
        #define vTexCUBEbias(_vtex_, _vsampl_, _vcoord_) _vtex_.Sample(_vsampl_, _vcoord_, _vcoord_.w)
        #define vTexCUBElod(_vtex_, _vsampl_, _vcoord_) _vtex_.SampleLevel(_vsampl_, _vcoord_, _vcoord_.w)
        #define vLoadTex2D(_vtex_, _vcoord_) _vtex_.Load(int3((int)_vcoord_.x, (int)_vcoord_.y, 0))
        
        #define DECLARE_TEX2D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          Texture2D <float4> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);
            
        #define DECLARE_TEX2D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          Texture2D <float> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);

        #define DECLARE_TEXCUBE_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          TextureCube <float4> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);
            
        #define DECLARE_TEXCUBE_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          TextureCube <float> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);

        #define DECLARE_TEX3D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
          Texture3D <float4> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);
            
        #define DECLARE_TEX3D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
          Texture3D <float> _texturename_ : register(t##_samplerregister_); \
          sampler _samplername_ : register(s##_samplerregister_);
       
      #endif
      
      #define UNROLL_LOOP [unroll]
    #else
    
      #if defined(_VISION_PS3)
        #define VFACING FACE
      #else
        #define VFACING VFACE
      #endif
      
      #define VFACING_TYPE float

      #define SV_Position POSITION
      #define SV_Target  COLOR
      #define SV_Target0 COLOR0
      #define SV_Target1 COLOR1
      #define SV_Target2 COLOR2
      #define SV_Target3 COLOR3
      #define SV_Target4 COLOR4
      #define SV_Target5 COLOR5
      #define SV_Target6 COLOR6
      #define SV_Target7 COLOR7

      #define vTex2D(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
      #if defined(_VISION_PSP2)
        #define vTex2D1Comp(_vtex_, _vsampl_, _vcoord_) tex2D<float>(_vtex_, _vcoord_)
        #define vTex2D2Comp(_vtex_, _vsampl_, _vcoord_) tex2D<float2>(_vtex_, _vcoord_)
        #define vTex2D3Comp(_vtex_, _vsampl_, _vcoord_) tex2D<float3>(_vtex_, _vcoord_)
        #define vTex2D4Comp(_vtex_, _vsampl_, _vcoord_) tex2D<float4>(_vtex_, _vcoord_)
        #define vTex2Dproj(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_.xy / _vcoord_.w)
      #else
        #define vTex2D1Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D2Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D3Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2D4Comp(_vtex_, _vsampl_, _vcoord_) tex2D(_vtex_, _vcoord_)
        #define vTex2Dproj(_vtex_, _vsampl_, _vcoord_) tex2Dproj(_vtex_, _vcoord_)
      #endif
      #define vTex2Dlod(_vtex_, _vsampl_, _vcoord_) tex2Dlod(_vtex_, _vcoord_)
      #define vTex3D(_vtex_, _vsampl_, _vcoord_) tex3D(_vtex_, _vcoord_)
      #define vTexCUBE(_vtex_, _vsampl_, _vcoord_) texCUBE(_vtex_, _vcoord_)
      #define vTexCUBEbias(_vtex_, _vsampl_, _vcoord_) texCUBEbias(_vtex_, _vcoord_)
      #define vTexCUBElod(_vtex_, _vsampl_, _vcoord_) texCUBElod(_vtex_, _vcoord_)
      #define vLoadTex2D(_vtex_, _vcoord_) tex2Dlod(_vtex_, float4(_vcoord_.x, _vcoord_.y, 0.0f, 0.0f))
      
      #define DECLARE_TEX2D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
        sampler2D _texturename_ : register(s##_samplerregister_);
        
      #define DECLARE_TEX2D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
        sampler2D _texturename_ : register(s##_samplerregister_);
      
      #define DECLARE_TEXCUBE_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
        samplerCUBE _texturename_ : register(s##_samplerregister_);
        
      #define DECLARE_TEXCUBE_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
        samplerCUBE _texturename_ : register(s##_samplerregister_);

      #define DECLARE_TEX3D_SAMPLER_4F(_texturename_, _samplername_, _samplerregister_) \
        sampler3D _texturename_ : register(s##_samplerregister_);
        
      #define DECLARE_TEX3D_SAMPLER_1F(_texturename_, _samplername_, _samplerregister_) \
        sampler3D _texturename_ : register(s##_samplerregister_);
        
      #if defined(_VISION_PS3) || defined(_VISION_PSP2) || defined(_VISION_WIIU)

        #define UNROLL_LOOP
        #define vTex2Dgrad(_vtex_, _vsampl_, _vcoord_, _ddx_, _ddy_) tex2D(_vtex_, _vcoord_, _ddx_, _ddy_)
      
      #else
      
        #define UNROLL_LOOP [unroll]
        #define vTex2Dgrad(_vtex_, _vsampl_, _vcoord_, _ddx_, _ddy_) tex2Dgrad(_vtex_, _vcoord_, _ddx_, _ddy_)
      
      #endif
      
    #endif


    #if defined(_VISION_GLES2) || defined(_VISION_WIIU)
        float trunc (float f)
        {
            return sign (f) * floor (abs (f));
        }
        float2 trunc (float2 f)
        {
            return sign (f) * floor (abs (f));
        }
        float3 trunc (float3 f)
        {
            return sign (f) * floor (abs (f));
        }
        float4 trunc (float4 f)
        {
            return sign (f) * floor (abs (f));
        }
    #endif
#endif
#if defined(_VISION_DX11) || defined(_VISION_PS3)
  #define SUPPORTS_SELECTIVE_BLENDING
#endif
