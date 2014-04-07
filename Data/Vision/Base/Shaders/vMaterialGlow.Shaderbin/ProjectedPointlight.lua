
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("DYNLIGHT_SPOT_PROJECTOR", false);
AddPermutationConst ("DYNLIGHT_OMNI_PROJECTOR", true);
AddPermutationConst ("DYNLIGHT_DIRECTIONAL", false);
AddPermutationBool ("VMATERIAL_NORMALMAP");
AddPermutationBool ("SHADOWMAP");
AddPermutationBool ("VMATERIAL_SPECULAR");
AddPermutationBool ("VMATERIAL_PARALLAX");
AddPermutationBool ("VMATERIAL_REFMAP");
AddPermutationConst ("DYNLIGHT_OMNI_PROJECTOR_DISTANCEMIPMAP", false);
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", true);
AddPermutationConst ("DYNLIGHT_ATTENUATION_ONE_OVER_X", false);
AddPermutationConst ("USE_FOG", true);
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


