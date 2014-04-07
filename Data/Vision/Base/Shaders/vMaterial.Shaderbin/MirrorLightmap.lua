
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("USE_MIRROR_CLIP_PLANE", true);
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


