
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("VMATERIAL_NORMALMAP", false);
AddPermutationConst ("VMATERIAL_SPECULAR", false);
AddPermutationConst ("VMATERIAL_PARALLAX", false);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", false);
AddPermutationConst ("USE_FOG", true);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationConst ("USE_MIRROR_CLIP_PLANE", true);
AddPermutationBool ("ALPHATEST");
AddPermutationConst ("VMATERIAL_REFMAP", false);


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



