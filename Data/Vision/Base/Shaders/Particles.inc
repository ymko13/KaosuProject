///////////////////////////////////////////////////////////
// Particles.inc
//
// Permutation variables: HWSPANNING, SMOOTHANIM, 
//                        ALPHATESTINSHADER
///////////////////////////////////////////////////////////

#include <Shaders/VisionCommon.inc>
#include <Shaders/ShaderHelpers.inc>
#include <Shaders/DeferredShadingHelpers.inc>

// Translate hw spanning permutation variable into preprocessor define
// to be also able to define it dynamically.
$ifdef HWSPANNING
  #define PARTICLES_USE_HARDWARESPANNING
$endif

// no instancing without hardware spanning
#ifdef PARTICLES_USE_INSTANCING
  #define PARTICLES_USE_HARDWARESPANNING
#endif

#ifdef PARTICLES_DEFERRED_OUTPUT
  #define PARTICLES_USE_NORMALMAP
#endif

// use instancing when geometry particles are used and hardware spanning
// is supported
#ifdef PARTICLES_GEOMETRY
  $ifdef HWSPANNING
    #define PARTICLES_USE_INSTANCING
  $endif
#endif

#ifdef _VISION_DX11
  cbuffer g_GlobalConstantBufferFrame : register (b0)
  {
    float4x4 matPJ         : packoffset(c0);   // projection matrix
    float4x4 matEye        : packoffset(c4);   // eye matrix
    float4x4 matInvEye     : packoffset(c8);   // inverse eye matrix
    float4 contextCP       : packoffset(c12);  // context clip planes: near, far, 1/(far-near)
    float3 cameraPosition  : packoffset(c17);  // worldspace camera position
    float4 gAmbientCol     : packoffset(c21);  // global ambient color
  }
  cbuffer g_GlobalConstantBufferObject : register (b1)
  {
    float4x4 matMV         : packoffset(c0);   // model view matrix
    float4x4 matMVP        : packoffset(c4);   // model view projection matrix
    float4x4 matM2WS       : packoffset(c8);   // model to world space matrix
  }

  #ifdef _IS_VERTEX_SHADER
    cbuffer g_GlobalConstantBufferParticles : register (b3)
    {
      float4   fadeParams    : packoffset(c0);  // particle fade parameter: near, far, 1/(far-near)
      float4   vSubDivAspect : packoffset(c1);  // particle animation parameter: xy:animsubdiv, z:aspect (but not used), w:repetitions 
      float4   vStepTexel    : packoffset(c2);  // particle animation parameter: xy:1/animsubdiv, yw:1/texturesize
      float4   vConfig       : packoffset(c3);  // particle flags: x=animated, y=usernormals, z=distorted, w=z-offset
    #if !defined(PARTICLES_USE_HARDWARESPANNING) && defined(PARTICLES_GEOMETRY)
      float4   vParticleColor: packoffset(c4);  // Used for particle color for non-HWS geometry particles
    #else
      float4   vCornerUV     : packoffset(c4);  // Corners: xy:upper left, zw: lower right. E.g. -0.5,-0.5,0.5,0.5.
    #endif
    }

    #ifdef DESATURATION
      StructuredBuffer<uint2> DesaturationColorIn : register(t3);
    #endif //DESATURATION

  #endif

  #ifdef _IS_PIXEL_SHADER
    cbuffer g_GlobalConstantBufferLightGrid : register (b3)
    {
      float4   Light0        : packoffset(c0);
      float4   Light1        : packoffset(c1);
      float4   Light2        : packoffset(c2);
      float4   Light3        : packoffset(c3);
      float4   Light4        : packoffset(c4);
      float4   Light5        : packoffset(c5);
    }
  #endif

  Texture2D BaseTexture         : register(t0);
  sampler   BaseTextureSampler  : register(s0);
  Texture2D Normalmap           : register(t1);
  sampler   NormalmapSampler    : register(s1);
  #ifdef PARTICLES_SOFT
    Texture2D DepthTexture        : register(t2);
    sampler   DepthTextureSampler : register(s2);
  #endif
  
#else

  #ifdef _IS_VERTEX_SHADER
    float4x4 matMV         : register(c0);
    float4x4 matPJ         : register(c4);
    float4x4 matMVP        : register(c8);
    float4x4 matEye        : register(c12);
    float4x4 matInvEye     : register(c16);
    float4x4 matM2WS       : register(c20);
    float4 contextCP       : register(c24);
    float4 cameraPosition  : register(c31);   // worldspace camera position

    float4 fadeParams      : register(c38);

    // hardware spanning
    float4 vSubDivAspect   : register(c39);
    float4 vStepTexel      : register(c40);
    float4 vConfig         : register(c41);
    #if !defined(PARTICLES_USE_HARDWARESPANNING) && defined(PARTICLES_GEOMETRY)
      float4 vParticleColor: register(c42);    // Used for particle color for non-HWS geometry particles
    #else
      float4 vCornerUV       : register(c42);  // Corners: xy:upper left, zw: lower right. E.g. -0.5,-0.5,0.5,0.5. Used for particle color for non-HWS geometry particles
    #endif 
    
    #ifdef _VISION_XENON
      float4 vScreenSpanU    : register(c16); // world matrix
      float4 vScreenSpanV    : register(c17); // world matrix
      float4 vDepth          : register(c18); // world matrix
    #endif

  #endif // _IS_VERTEX_SHADER

  #ifdef _IS_PIXEL_SHADER
    // deferred lighting
    float3 gAmbientCol     : register(c21);
    
    // lightgrid lighting
    float4 Light0          : register(c26);
    float4 Light1          : register(c27);
    float4 Light2          : register(c28);
    float4 Light3          : register(c29);
    float4 Light4          : register(c30);
    float4 Light5          : register(c31);

    sampler2D BaseTexture  : register(s0);
    sampler2D Normalmap    : register(s1);
    sampler2D DepthTexture : register(s2);
  #endif // _IS_PIXEL_SHADER
  
#endif

// vertex out - same for all platforms. Also used by ps_main
struct VS_OUT
{
  float4 ProjPos  : SV_Position;
  #ifdef PARTICLES_DEFERRED_OUTPUT
    float3 UV : TEXCOORD0; // z=depth
  #else
    float2 UV : TEXCOORD0;
  #endif
    
  #ifdef _VISION_XENON
    float4 Color    : COLOR0_center;
  #else  
    float4 Color    : COLOR;
  #endif

  $if defined(SMOOTHANIM) && defined(HWSPANNING) // no smooth anim without hw spanning
    float3 UVBlend : TEXCOORD1; // encodes UV and blend factor
  $endif

  #ifdef PARTICLES_SOFT
    float4 SoftParams : TEXCOORD2; // xy=proj coords, z=depth, w=farclip/radius
  #endif

  #ifdef PARTICLES_USE_PROJECTION
    float2 UVProj  : TEXCOORD2;
  #endif
    
  #ifdef PARTICLES_USE_NORMALMAP
    float3 Normal  : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
    float3 BiTangent: TEXCOORD5;
  #endif

  #ifdef PARTICLES_USE_EYE_DIR
    float3 EyeDir  : TEXCOORD6;
  #endif
};



#ifdef _IS_VERTEX_SHADER

  /////////////////////////////////////////////////////////////////////////////////
  // this function uses the projection matrix to map the [-1..1] vertex position 
  // to frame buffer texture coordinates
  float2 GetProjectedCoords(float4 projPos)
  {
    float2 aligned = (projPos.xy / projPos.w);
    #if defined(_VISION_GLES2)
      return aligned * 0.5 + 0.5;
    #else
      return aligned * float2(0.5, -0.5) + 0.5;
    #endif
  }


  // calculate particle fading value
  float GetFadeScalar(float z)
  {
    return saturate(1.0-(z-fadeParams.x)*fadeParams.z);
  }

  float3 ApplyDepthOffset(float3 ws, float fFactor)
  {
  //  float3 camDir = normalize(cameraPosition-ws);
  //  return ws + camDir*fUnits;
    float3 camDir = cameraPosition-ws;
    return ws + fFactor*camDir; // percentage fraction
  }


  #ifndef PARTICLES_CUSTOM_UV_SWIZZLE
    #define SwizzleUV(_in) _in
  #endif



  // calculate tile UV for given frame index
  float2 GetAnimFrameUV(float fFrame, float2 corner)
  {
    #if defined(_VISION_PSP2)
      float t = fFrame / vSubDivAspect.x;
      float2 v1 = float2(frac(t) * vSubDivAspect.x, floor(t));
    #else
      float2 v1;
      v1.x = modf(fFrame / vSubDivAspect.x, v1.y) * vSubDivAspect.x;
    #endif
    
    v1 *= vStepTexel.xy;
    float2 v2 = float2(v1.x+vStepTexel.x-vStepTexel.z, v1.y+vStepTexel.y-vStepTexel.w);
    v1 += vStepTexel.zw;
    float2 res = float2(
      corner.x * v2.x + (1.0f-corner.x) * v1.x,
      corner.y * v2.y + (1.0f-corner.y) * v1.y);
    
    return res;
  }


  #ifdef PARTICLES_USE_NORMALMAP

    void BuildTangentSpace(out float3 Tangent, out float3 BiTangent, out float3 Normal, float3 vScreenSpanU, float3 vScreenSpanV)
    {
    /*
      // the following is correct for all platforms
      Tangent   = matInvEye._m00_m10_m20_m30;  // camera right
      BiTangent = matInvEye._m01_m11_m21_m31;  // camera up
      Normal    = -matInvEye._m02_m12_m22_m32; // camera dir
     */
     Tangent = normalize(vScreenSpanU);
     BiTangent = normalize(vScreenSpanV);
     Normal = normalize(cross(vScreenSpanV,vScreenSpanU));
    }

  #endif

  #ifdef PARTICLES_GEOMETRY
  
  // the following define can be used in custom code do define a custom MatrixFromAxisAngle function to return a custom 3x3 rotation matrix for geometry particles.
  // Function arguments are the rotation axis and the rotation angle (rad) around it
  #ifndef PARTICLES_GEOMETRY_CUSTOM_ROTATION_MATRIX
  
    // used for geometry rotation:
    float3x3 MatrixFromAxisAngle(float3 dir, float fAngle)
    {
      float s, c;
      float xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

      sincos(fAngle, s,c);

      xx = dir.x * dir.x;
      yy = dir.y * dir.y;
      zz = dir.z * dir.z;
      xy = dir.x * dir.y;
      yz = dir.y * dir.z;
      zx = dir.z * dir.x;
      xs = dir.x * s;
      ys = dir.y * s;
      zs = dir.z * s;
      one_c = 1.0f - c;

      float3x3 m;
      m._m00 = (one_c * xx) + c;
      m._m01 = (one_c * xy) - zs;
      m._m02 = (one_c * zx) + ys;

      m._m10 = (one_c * xy) + zs;
      m._m11 = (one_c * yy) + c;
      m._m12 = (one_c * yz) - xs;
      
      m._m20 = (one_c * zx) - ys;
      m._m21 = (one_c * yz) + xs;
      m._m22 = (one_c * zz) + c;
      return m;
    }

  #endif // ifndef PARTICLES_GEOMETRY_CUSTOM_ROTATION_MATRIX
  #endif // PARTICLES_GEOMETRY


  #ifdef PARTICLES_USE_HARDWARESPANNING

    #if defined(_VISION_PS3) || defined(_VISION_PSP2)

      struct VS_IN
      {
        #ifdef PARTICLES_GEOMETRY
          float3 MeshPosition : POSITION;   // mesh data
          float4 Color        : COLOR;
          float2 UV0           : TEXCOORD0; // mesh data
          float4 ObjPosAndSize : TEXCOORD3; // center.xyz, w=size
          float3 Distortion    : TEXCOORD4; // x=angle
          float3 RotationAxis  : TEXCOORD5; // Rotation
          float4 AnimFrame     : TEXCOORD6; // x=frame#1, y=frame#2, z=weight
        #else // billboards:
          float4 ObjPosAndSize : POSITION;
          float3 Normal : NORMAL;
          float4 Color    : COLOR;
          float3 Distortion : TEXCOORD0;
          float3 AnimFrame : TEXCOORD1;
          float2 Corners : TEXCOORD2;
        #endif
      };

    #else

      struct VS_IN
      {
        #ifdef PARTICLES_GEOMETRY
          float3 MeshPosition : POSITION;   // mesh data
          
          #ifdef _VISION_XENON
            float4 Color    : COLOR0_center;
          #else  
            float4 Color    : COLOR;
          #endif
          float2 UV0           : TEXCOORD0; // mesh data
          float4 ObjPosAndSize : TEXCOORD3; // center.xyz, w=size
          float3 Distortion    : TEXCOORD4; // x=angle
          float3 RotationAxis  : TEXCOORD5; // Rotation
          float4 AnimFrame     : TEXCOORD6; // x=frame#1, y=frame#2, z=weight
        #else // billboards:
          float2 Corners : POSITION;
          float3 Normal : NORMAL;
          #ifdef _VISION_XENON
            float4 Color    : COLOR0_center;
          #else  
            float4 Color    : COLOR;
          #endif
          float3 Distortion : TEXCOORD0;
          float4 AnimFrame : TEXCOORD1; // x=frame#1, y=frame#2, z=weight
          float4 ObjPosAndSize : TEXCOORD2; // center.xyz, w=size

          #ifdef DESATURATION
            uint InstanceID        : SV_InstanceID;
          #endif //DESATURATION
        #endif
      };

    #endif


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // All platforms
    ///////////////////////////////////////////////////////////////////////////////////////////////

    #if defined(_VISION_XENON)
      VS_OUT vs_main( int i : INDEX )
    #else
      VS_OUT vs_main( VS_IN In )
    #endif
    {
      VS_OUT Out;
      
      // setup aliases used in the shader:
      float4 ObjPosAndSize;
      float2 Corner;
      float4 Color;
      float4 Distortion;
      float4 Normal;
      float4 AnimFrame; // x=frame#1, y=frame#2, z=weight
      float4 RotationAxis;
        
      #if defined(_VISION_XENON)
        #ifdef PARTICLES_GEOMETRY
          float3 MeshPosition;
          // Need some constructive casting so 360 compiler doesn't optimize incorrectly
          int iVertexCount = (int)vCornerUV.x;
          int iParticleIndex = (int)( (i+0.5f) / iVertexCount );
          int iGeometryVertexIndex = i - (iParticleIndex * iVertexCount);
          asm
          {
            // Fetch the Mesh Geometry Vertex Data
            vfetch MeshPosition.xyz, iGeometryVertexIndex, position0;
            vfetch Corner.xy, iGeometryVertexIndex, texcoord0;

            // Fetch the Instance/Particle Data 
            vfetch ObjPosAndSize, iParticleIndex, texcoord3;
            vfetch Distortion, iParticleIndex, texcoord4;
            vfetch RotationAxis.xyz, iParticleIndex, texcoord5;
            vfetch AnimFrame, iParticleIndex, texcoord6;
            vfetch Color, iParticleIndex, color0;
            vfetch Normal, iParticleIndex, normal; // TODO: Should normal come from geometry in this case?
          };
        #else
          float2 Corners[4];
          Corners[0].x = 0.0f; Corners[0].y = 1.0f;
          Corners[1].x = 1.0f; Corners[1].y = 1.0f;
          Corners[2].x = 1.0f; Corners[2].y = 0.0f;
          Corners[3].x = 0.0f; Corners[3].y = 0.0f;

          int iDiv = i / 4;
          int iCornerIndex = i - (iDiv * 4);

          // Fetch the particle data using iDiv.
          asm
          {
            vfetch ObjPosAndSize, iDiv, position0;
            vfetch Distortion, iDiv, texcoord0;
            vfetch AnimFrame, iDiv, texcoord1;
            vfetch Color, iDiv, color0;
            vfetch Normal, iDiv, normal;
          };

          Corner = Corners[iCornerIndex].xy;
        #endif
      #else
        // default setup:
        #ifdef PARTICLES_GEOMETRY
          float3 MeshPosition = In.MeshPosition;
          ObjPosAndSize = In.ObjPosAndSize;
          Color = In.Color;
          Distortion.xyz = In.Distortion;
          AnimFrame.xyz = In.AnimFrame.xyz;
          Corner = In.UV0;
          RotationAxis.xyz = In.RotationAxis;
        #else
          ObjPosAndSize = In.ObjPosAndSize;
          Corner = In.Corners;
          Color = In.Color;
          Distortion.xyz = In.Distortion;
          Normal.xyz = In.Normal;
          AnimFrame.xyz = In.AnimFrame.xyz;
        #endif // PARTICLES_GEOMETRY
      #endif
        
      
      
      // "vDepth" is the negated camera view direction in worldspace
      float4 vDepth = matInvEye._m02_m12_m22_m32;
      
      #ifdef PARTICLES_CUSTOM_SPAN_VECTORS
        float3 vScreenSpanV = CustomSpanVectorV; // use defines in custom code
        float3 vScreenSpanU = CustomSpanVectorU;
      #elif defined (PARTICLES_ALWAYS_UP)
        float3 vScreenSpanV = float3(0,0,1); // up vector
        float3 vScreenSpanU = normalize(cross(vScreenSpanV,vDepth.xyz));
      #elif defined (PARTICLES_SPAN_DIRECTION)
        float3 vPosDir = ObjPosAndSize.xyz - cameraPosition.xyz;
        float3 vScreenSpanV = matInvEye._m01_m11_m21_m31; // start spanning with camera up
        float3 vScreenSpanU = normalize(cross(vScreenSpanV,vPosDir.xyz));
        vScreenSpanV = normalize(cross(vPosDir.xyz,vScreenSpanU));
      #else
        float4 vScreenSpanU = matInvEye._m00_m10_m20_m30;
        float4 vScreenSpanV = matInvEye._m01_m11_m21_m31;
      #endif

      // Copy Color
      Out.Color = Color;

      // Compute TexCoord
      if (vConfig.x > 0.0f)
      {
        Out.UV.xy = SwizzleUV(GetAnimFrameUV(AnimFrame.x*255.0f + 0.001, Corner));
        $if defined(SMOOTHANIM) && defined(HWSPANNING) // no smooth anim without hw spanning
          Out.UVBlend.xy = SwizzleUV(GetAnimFrameUV(AnimFrame.y*255.0f + 0.001, Corner));
          Out.UVBlend.z = AnimFrame.z; // blend value
        $endif
      }
      else
      {
        Out.UV.xy = SwizzleUV(Corner.xy * float2(vSubDivAspect.w, 1.0f));
        $if defined(SMOOTHANIM) && defined(HWSPANNING) // no smooth anim without hw spanning
          Out.UVBlend = 0;
        $endif
      }

      // Compute Position
      float3 vSpanVec;
      float4 vWorldPos;
      
      #ifdef PARTICLES_GEOMETRY
        float3x3 rotation = MatrixFromAxisAngle(RotationAxis.xyz, Distortion.x);
        vWorldPos.xyz = mul(rotation, MeshPosition) * ObjPosAndSize.w + ObjPosAndSize.xyz;
        vWorldPos.w = 1.0f;
      #else // PARTICLES_GEOMETRY:

        #ifdef PARTICLES_USE_LOCALSPACE
          float3 vNormal = (vConfig.y > 0.0f) ? mul((float3x3)matM2WS, Normal) : vDepth.xyz;
        #else
          float3 vNormal = (vConfig.y > 0.0f) ? Normal.xyz : vDepth.xyz;
        #endif
      
        // Corner.xy is in the local particle space (relative to its "center")
        float2 vSpanFactors = lerp(vCornerUV.xy,vCornerUV.zw,Corner.xy);
         
        if (vConfig.z > 0.0f)   // use distortion
        {
          #ifdef PARTICLES_USE_LOCALSPACE
            vWorldPos.xyz = mul( matM2WS, float4(ObjPosAndSize.xyz,1.0) );
            float3 vDistort = mul((float3x3)matM2WS, Distortion.xyz);
          #else
            vWorldPos.xyz = ObjPosAndSize.xyz;
            // "vDistort" is the vector from this particle to the next particle in the trail
            float3 vDistort = Distortion.xyz;
          #endif
          
          // Determine the extrude direction of the particle (in worldspace)
          #ifndef PARTICLES_TRAIL
            vSpanVec = normalize(cross(vDistort.xyz, vNormal));
          #else
            // Use the view direction from the camera to the particle to span the particle plane
            float3 vCameraToParticle = ObjPosAndSize.xyz - cameraPosition.xyz;
            float3 vParticleTravelDirection = vDistort.xyz;
            
            // In trail mode we need different corner vectors on the right half of the distorted particle. The simulation tracks this into the normal vector
            // "Normal" is the vector from this particle's next particle to that particle's next particle (it's used to "fix up" the right half of the vertices 
            // of the current particle to match the left half vertices of the next particle)
            if(Corner.x > 0.5)
            {
              // The view direction to the next particle is the view direction to the current particle plus the displacement from the current particle to the next.
              vCameraToParticle += vParticleTravelDirection.xyz;
              vParticleTravelDirection = Normal.xyz;
            }
            
            // The cross product will only fail if the view direction to the particle is collinear to the particle's travel direction.
            // This case is so rare that we ignore it to safe performance.
            vSpanVec = normalize(cross(vParticleTravelDirection, vCameraToParticle));
          #endif
          
          vSpanVec *= ObjPosAndSize.w;
          
          vWorldPos.xyz += vSpanVec * vSpanFactors.y + vDistort * vSpanFactors.x;
          vWorldPos.w = 1.0f;
          
          #ifdef PARTICLES_USE_NORMALMAP
            BuildTangentSpace(Out.Tangent,Out.BiTangent,Out.Normal, vDistort.xyz, vSpanVec.xyz);
          #endif
        }
        else
        {
          float3 vUSpan0 = vScreenSpanU.xyz * ObjPosAndSize.w;
          float3 vVSpan0 = vScreenSpanV.xyz * ObjPosAndSize.w;
          
          // do the rotation (angle is in distortion, because distortion isn't needed here)
          float sw = sin(Distortion.x);
          float cw = cos(Distortion.x);
          float3 vUSpan = vUSpan0*cw - vVSpan0*sw;
          float3 vVSpan = vUSpan0*sw + vVSpan0*cw;
          
          #ifdef PARTICLES_USE_LOCALSPACE
            vWorldPos.xyz = mul( matM2WS, float4(ObjPosAndSize.xyz,1.0) );
          #else
            vWorldPos.xyz = ObjPosAndSize.xyz;
          #endif
            vWorldPos.xyz += vUSpan * vSpanFactors.x + vVSpan * vSpanFactors.y;
            vWorldPos.w = 1.0f;
          #ifdef PARTICLES_USE_NORMALMAP
            BuildTangentSpace(Out.Tangent,Out.BiTangent,Out.Normal, vUSpan.xyz, vVSpan.xyz);
          #endif
        }
      #endif // PARTICLES_GEOMETRY

      #ifdef PARTICLES_EXPOSE_Z_OFS
         vWorldPos.xyz = ApplyDepthOffset(vWorldPos.xyz, vConfig.w);
      #endif

      #ifdef PARTICLES_USE_EYE_DIR
        // Normalize in PS
        Out.EyeDir = cameraPosition-vWorldPos.xyz;
      #endif

      #ifdef PARTICLES_USE_LOCALSPACE
        float4 vESPos = mul( matEye, vWorldPos );
        Out.ProjPos = mul( matPJ, vESPos );
      #else
        // Transform Position                        
        Out.ProjPos = mul( matMVP, vWorldPos );
      #endif
      
      #ifdef PARTICLES_USE_PROJECTION
         Out.UVProj = GetProjectedCoords(Out.ProjPos);
      #endif

      #ifdef PARTICLES_DEFERRED_OUTPUT
        Out.UV.z = Out.ProjPos.z / contextCP.y; // depth
      #endif

      #ifdef PARTICLES_SOFT
        Out.SoftParams.xy = GetProjectedCoords(Out.ProjPos);
        Out.SoftParams.z = mul( matMV, vWorldPos ).z * contextCP.z; // calculate linear depth value, which is compared againsted the linear depth from the texture in the pixel shader
        Out.SoftParams.w = contextCP.y / ObjPosAndSize.w;
      #endif

      // fade
      Out.Color.a *= GetFadeScalar(Out.ProjPos.z);

      #ifdef DESATURATION
        #ifdef PARTICLES_GEOMETRY
          uint iID = 0;
        #else
          uint iID = In.InstanceID;
        #endif

        float3 cDesaturationColor = GetPointDesaturation(DesaturationColorIn, iID);
        float fDesaturationLuminance = cDesaturationColor.r * 0.299f + cDesaturationColor.g * 0.587f + cDesaturationColor.b * 0.114f;

        Out.Color.a *= fDesaturationLuminance;
      #endif //DESATURATION

      return Out;
    }

  #else // CPU spanning; same for all version

    struct VS_IN
    {
      float3 ObjPosAndSize  : POSITION;
      float4 Color   : COLOR;
      float2 UV      : TEXCOORD0;

      #ifdef DESATURATION
        uint VertexID       : SV_VertexID;
      #endif //DESATURATION
    };


    VS_OUT vs_main( VS_IN In )
    {
      VS_OUT Out;
      float4 vWorldPos = float4(In.ObjPosAndSize.xyz, 1.0f);
      #ifdef PARTICLES_EXPOSE_Z_OFS
         vWorldPos.xyz = ApplyDepthOffset(vWorldPos.xyz, vConfig.w);
      #endif
        Out.ProjPos = mul( matMVP, vWorldPos);
        Out.UV.xy = SwizzleUV(In.UV.xy);
      $if defined(SMOOTHANIM) && defined(HWSPANNING) // no smooth anim without hw spanning
        Out.UVBlend = 0;
      $endif

      #if !defined(PARTICLES_USE_HARDWARESPANNING) && defined(PARTICLES_GEOMETRY)
        Out.Color = vParticleColor; 
      #else
        Out.Color = In.Color;
      #endif
      Out.Color.a *= GetFadeScalar(Out.ProjPos.z);

      #ifdef DESATURATION
        float3 cDesaturationColor = GetPointDesaturation(DesaturationColorIn, In.VertexID / 6);
        float fDesaturationLuminance = cDesaturationColor.r * 0.299f + cDesaturationColor.g * 0.587f + cDesaturationColor.b * 0.114f;

        Out.Color.a *= fDesaturationLuminance;
      #endif //DESATURATION

      #ifdef PARTICLES_USE_NORMALMAP
        BuildTangentSpace(Out.Tangent,Out.BiTangent,Out.Normal, matInvEye._m00_m10_m20, matInvEye._m01_m11_m21);
      #endif

      #ifdef PARTICLES_USE_PROJECTION
         Out.UVProj = GetProjectedCoords(Out.ProjPos);
      #endif
      
      return Out;
    }

  #endif // PARTICLES_USE_HARDWARESPANNING
#endif //_IS_VERTEX_SHADER


#ifdef _IS_PIXEL_SHADER

  ///////////////////////////////////////////////////////////
  // pixel shader: same for all modes/platforms
  ///////////////////////////////////////////////////////////


  #ifdef PARTICLES_DEFERRED_OUTPUT
    PS_OUT ps_main( VS_OUT In )
  #else
    float4 ps_main( VS_OUT In ) : SV_Target
  #endif
  {
     
    $if defined(SMOOTHANIM) && defined(HWSPANNING) // no smooth anim without hw spanning
      float4 frame0 = vTex2D(BaseTexture, BaseTextureSampler, In.UV.xy);
      float4 frame1 = vTex2D(BaseTexture, BaseTextureSampler, In.UVBlend.xy);
      float4 texColor = lerp(frame0,frame1,In.UVBlend.z);
    $else
      float4 texColor = vTex2D(BaseTexture, BaseTextureSampler, In.UV.xy);
    $endif 

    texColor *= In.Color;

    #ifdef PARTICLES_SOFT
      float fDepth = READ_CONVERTED_DEPTH (DepthTexture, DepthTextureSampler, In.SoftParams.xy).r;
      float fSoftFade = saturate((fDepth - In.SoftParams.z) * In.SoftParams.w);
      texColor.a *= fSoftFade;
    #endif

    $ifdef ALPHATESTINSHADER
      clip(texColor.a - 0.25f);
    $else
      #ifdef PARTICLES_DEFERRED_OUTPUT
        clip(texColor.a - 0.001);
      #endif
    $endif

    #ifndef PARTICLES_USE_NORMALMAP
      // simple diffuse*color
      return texColor;
    #else
      // color from normalmap & lightgrid
      float4 vNormal = vTex2D(Normalmap, NormalmapSampler, In.UV.xy);
      vNormal.xyz = vNormal.xyz*2.0-1.0;
      float3 vWSNormal = GetWorldNormal(vNormal, In.Tangent, In.BiTangent, In.Normal);
      
      #ifdef PARTICLES_DEFERRED_OUTPUT
        PixelDataUnpacked pixelData;
        pixelData.vAccInitial = float3(0,0,0); // ambient
        pixelData.fDepth = In.UV.z;
        pixelData.vNormal = vWSNormal;
        pixelData.vDiffColor = texColor.xyz;
        pixelData.vSpecColor = float3(0,0,0);
        pixelData.fSpecExp = 1.0;
        pixelData.fMaterial = 1.0;
        return packPixelValues(pixelData);  
      #else
        float4 lighting = GetLightGridDiffuse_WS(vWSNormal, Light0,Light1,Light2,Light3,Light4,Light5);
        return lighting*texColor;
      #endif
    #endif
  }

#endif // _IS_PIXEL_SHADER
