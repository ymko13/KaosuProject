
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("ALPHATEST");
AddPermutationBool ("VMATERIAL_REFMAP");
AddPermutationBool ("VMATERIAL_NORMALMAP");
AddPermutationBool ("VMATERIAL_PARALLAX");
AddPermutationBool ("MSAA");

AddPermutationConst ("LIGHTMAP", false);
AddPermutationConst ("LIGHTGRID", true);
AddPermutationConst ("VERTEXCOLOR", false);
AddPermutationConst ("LIGHTMAPDOT3", false);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", true);
AddPermutationConst ("FULLBRIGHT", false);
AddPermutationConst ("DEFERRED_OUTPUT", true);
AddPermutationConst ("VMATERIAL_PARALLAX_DEPTH_DISPLACEMENT", false);



--   Filtering-Function to quickly remove unnecessary permutations:

function IsPermutationSupported (PermutationValues, Platform)

  if (PermutationValues["MSAA"] == true) and (Platform != DX11) then
    return false;
  end
  
  if (PermutationValues["VMATERIAL_NORMALMAP"] == false) then
  
    -- Parallax Mapping needs the NormalMap
    if (PermutationValues["VMATERIAL_PARALLAX"] == true) then
      return false;
    end
    
  end

  return true;
end
