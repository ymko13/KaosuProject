
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("LIGHTMAP", true);
AddPermutationConst ("MIRROR", false);
AddPermutationConst ("VMATERIAL_SPECULAR", true);
AddPermutationConst ("VMATERIAL_PARALLAX", false);
AddPermutationConst ("VMATERIAL_CUBE_REFLECTION", false);
AddPermutationConst ("USE_FOG", true);
AddPermutationConst ("VMATERIAL_NORMALMAP", false);
AddPermutationBool ("VMATERIAL_REFMAP");
AddPermutationConst ("USE_RIMLIGHT", true);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_GLOWMAP_ADDITIVE", false);
AddPermutationBool ("ALPHATEST");
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", true);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_BLENDED_NORMALS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_SSSSS", false);
AddPermutationConst ("VMATERIAL_SKIN_SHADING_PREINTEGRATED", false);


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


