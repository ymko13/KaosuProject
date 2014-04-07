
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("LIGHTMAP", true);
AddPermutationConst ("VMATERIAL_PARALLAX", true);
AddPermutationConst ("VMATERIAL_NORMALMAP", true);
AddPermutationConst ("VMATERIAL_SPECULAR", true);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", true);
AddPermutationConst ("USE_RIMLIGHT", false);
AddPermutationConst ("USE_FOG", true);
AddPermutationConst ("MIRROR", false);
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", true);
AddPermutationConst ("VMATERIAL_REFMAP", true);
AddPermutationConst ("VMATERIAL_GLOWMAP", true);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationConst ("ALPHATEST", false);


--   Filtering-Function to remove unnecessary permutations:

function IsPermutationSupported (PermutationValues, Platform)


--if (PermutationValues["VAR_NAME"] > some_value) and (Platform == DX9 / DX11 / XBOX360 / PS3 / PSP2 / GLES2) then
--  return false;
--end

--if (PermutationValues["VAR_NAME"] ~= some_value) and/or (PermutationValues["VAR_NAME"] == some_other_value) then
--  return false;
--end

  return true;
end


