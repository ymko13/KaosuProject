
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("VMATERIAL_NORMALMAP", false);
AddPermutationConst ("VMATERIAL_SPECULAR", false);
AddPermutationConst ("VMATERIAL_PARALLAX", false);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", false);
AddPermutationConst ("USE_RIMLIGHT", false);
AddPermutationConst ("USE_FOG", false);
AddPermutationConst ("MIRROR", false);
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", false);
AddPermutationConst ("VMATERIAL_REFMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_SSSSS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_BLENDED_NORMALS", false);
AddPermutationConst ("ALPHATEST", false);
AddPermutationBoolGroup("LIGHTGRID", "LIGHTGRIDSIMPLE");


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


