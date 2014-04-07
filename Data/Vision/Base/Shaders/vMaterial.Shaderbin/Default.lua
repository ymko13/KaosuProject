
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("MIRROR");
AddPermutationConst ("LIGHTMAP", false);
AddPermutationConst ("VMATERIAL_SPECULAR", false);
AddPermutationConst ("VMATERIAL_PARALLAX", false);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", false);
AddPermutationConst ("USE_FOG", true);
AddPermutationConst ("VMATERIAL_NORMALMAP", false);
AddPermutationConst ("VMATERIAL_REFMAP", false);
AddPermutationConst ("USE_RIMLIGHT", false);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationBool ("ALPHATEST");
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", true);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_BLENDED_NORMALS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_SSSSS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_PREINTEGRATED", false);


function IsPermutationSupported (PermutationValues)
  return true;
end


