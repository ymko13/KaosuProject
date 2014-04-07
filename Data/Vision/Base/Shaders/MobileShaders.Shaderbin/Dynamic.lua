
-- *** Vision Shader-Permutation Config-File *** 

-- vertex lighting and dot3 normal mapping cannot be active at the same time
-- place MOBILE_DEFAULT at the end (will be looked at first)
useFog = string.find(EffectName, "Fog") ~= nil;

AddPermutationBoolGroup("MOBILE_VERTEXLIGHTING", "MOBILE_DYNAMICLIGHTNORMALMAPPING", "MOBILE_DEFAULT")

AddPermutationBoolGroup("POINTLIGHT", "SPOTLIGHT", "DIRECTIONALLIGHT")

AddPermutationBool("USE_SHADOWMAP");

AddPermutationConst("NOFOG", not useFog);

AddPermutationConstAndIncTag("ALPHATEST", true);

AddPermutationConst("MOBILE_SUBTRACTIVE_SHADOWMAP", false);

--AddPermutationBool("SHADOWMAP")

--AddPermutationBool("VMATERIAL_SPECULAR")
