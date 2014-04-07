
-- *** Vision Shader-Permutation Config-File *** 


--   Functions to add Permutation-Variables and their valid Values:

-- AddPermutationBool      ("VAR_NAME");
-- AddPermutationBoolGroup ("VAR_NAME1", "VAR_NAME2", "VAR_NAME3", ...);
-- AddPermutationInt       ("VAR_NAME", int1, int2, int3, ...);
-- AddPermutationConst     ("VAR_NAME", true/false);
-- AddPermutationConst     ("VAR_NAME", int);


--   The following Permutation-Variables were extracted from the Shader:

useAnimated = string.find(EffectName, "Animated") ~= nil;
useCubemap = string.find(EffectName, "Cubemap") ~= nil;
useBump = string.find(EffectName, "Bump") ~= nil;
useGlow = string.find(EffectName, "Glow") ~= nil;
useFog = string.find(EffectName, "Fog") ~= nil;

isStatic = TechniqueName == "Static";
isCompound = TechniqueName == "Compound";
isSubtractive = TechniqueName == "Subtractive";

AddPermutationConst("VMATERIAL_BUMPCUBEMAP", useCubemap and useBump);
AddPermutationConst("VMATERIAL_CUBEMAP", useCubemap);
AddPermutationConst("VMATERIAL_BUMP", useBump);
AddPermutationConst("VMATERIAL_GLOW", useGlow);
AddPermutationConst("VMATERIAL_BUMPANIMATED", useBump and useAnimated);

AddPermutationConst("MOBILE_DYNAMICBASEPASSLIGHTING", isCompound);
AddPermutationConst("MOBILE_SUBTRACTIVE_SHADOWMAP", isSubtractive);

if isCompound then
  -- vertex lighting and dot3 normal mapping cannot be active at the same time
  -- place MOBILE_DEFAULT at the end (will be looked at first)
  AddPermutationBoolGroup("MOBILE_VERTEXLIGHTING", "MOBILE_DYNAMICLIGHTNORMALMAPPING", "MOBILE_DEFAULT");

  AddPermutationBoolGroup("POINTLIGHT", "SPOTLIGHT", "DIRECTIONALLIGHT");
  
  AddPermutationBool("USE_SHADOWMAP");
  
elseif isSubtractive then
  AddPermutationConst("MOBILE_VERTEXLIGHTING",false);
  AddPermutationConst("MOBILE_DYNAMICLIGHTNORMALMAPPING", false);
  AddPermutationConst("MOBILE_DEFAULT",false);
  
  AddPermutationBoolGroup("POINTLIGHT", "SPOTLIGHT", "DIRECTIONALLIGHT");
  
  AddPermutationConst("USE_SHADOWMAP",true);
  
else

  AddPermutationConst("MOBILE_VERTEXLIGHTING",false);
  AddPermutationConst("MOBILE_DYNAMICLIGHTNORMALMAPPING", false);
  AddPermutationConst("MOBILE_DEFAULT",false);

  AddPermutationConst("POINTLIGHT",false);
  AddPermutationConst("SPOTLIGHT",false);
  AddPermutationConst("DIRECTIONALLIGHT",false);
  
  AddPermutationConst("USE_SHADOWMAP",false);
end

if string.find(EffectName, "FakeSpecular") then
  AddPermutationConst("VMATERIAL_FAKESPECULAR", true);
  AddPermutationBool("VMATERIAL_REFMAP");
else
  AddPermutationConst("VMATERIAL_FAKESPECULAR", false);
  AddPermutationConst("VMATERIAL_REFMAP", true);
end

AddPermutationConst("NOFOG", not useFog);
AddPermutationConstAndIncTag("MIRROR", true);

AddPermutationBool("ALPHATEST");

AddPermutationBool("LIGHTMAP");
AddPermutationBool("LIGHTMAPDOT3");
AddPermutationBool("LIGHTGRIDSIMPLE");
AddPermutationBool("LIGHTGRID");

if isCompound then
  AddPermutationConst("FULLBRIGHT", false);
else
  AddPermutationBool("FULLBRIGHT");
end

AddPermutationBool("USE_GLOBAL_AMBIENT");

--   Filtering-Function to remove unnecessary permutations:

function IsPermutationSupported (PermutationValues, Platform)

-- Let LIGHTMAP also provide the LIGHTMAPDOT3 inclusion tag so that the shader can be selected by a non-mobile shader provider
if (PermutationValues["LIGHTMAPDOT3"] ~= PermutationValues["LIGHTMAP"]) then
  return false;
end

-- Let LIGHTGRIDSIMPLE also provide the LIGHTGRID inclusion tag so that the shader can be selected by a non-mobile shader provider
if (PermutationValues["LIGHTGRID"] ~= PermutationValues["LIGHTGRIDSIMPLE"]) then
  return false;
end

-- Ensure that either LIGHTMAP, LIGHTGRIDSIMPLE or FULLBRIGHT or none of them is active
local count = 0
if PermutationValues["LIGHTMAP"] then count = count + 1 end
if PermutationValues["LIGHTGRIDSIMPLE"] then count = count + 1 end
if PermutationValues["FULLBRIGHT"] then count = count + 1 end

if count > 1 then
  return false;
end

-- Shadow mapping not supported for point lights in mobile shaders
if ((PermutationValues["USE_SHADOWMAP"] == true) and (PermutationValues["POINTLIGHT"] == true)) then
  return false;
end

  return true;
end


