
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("VMATERIAL_NORMALMAP");
AddPermutationBool ("VMATERIAL_SPECULAR");
AddPermutationBool ("VMATERIAL_PARALLAX");
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", false);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", false);
AddPermutationConst ("USE_FOG", true);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationConst ("USE_RIMLIGHT", false);
AddPermutationConst ("LIGHTGRID", false);
AddPermutationConst ("LIGHTGRIDSIMPLE", true);
AddPermutationBool ("MIRROR");
AddPermutationBool ("ALPHATEST");
AddPermutationBool ("VMATERIAL_REFMAP");
AddPermutationConst ("VMATERIAL_SKIN_SHADING_BLENDED_NORMALS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_SSSSS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_PREINTEGRATED", false);


--   Filtering-Function to quickly remove unnecessary permutations:

function IsPermutationSupported (PermutationValues)

  if ((PermutationValues["VMATERIAL_NORMALMAP"] == false) and
      (PermutationValues["VMATERIAL_PARALLAX"] == true)) then
        return false;
  end
  
  if ((PermutationValues["VMATERIAL_REFMAP"] == true) and
      (PermutationValues["VMATERIAL_SPECULAR"] == false) and
      (PermutationValues["VMATERIAL_CUBE_REFLECTION"] == false)) then
    return false;
  end

  return true;
end



