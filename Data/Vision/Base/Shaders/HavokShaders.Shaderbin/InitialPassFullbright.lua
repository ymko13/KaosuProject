
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("ALPHATEST");
AddPermutationBool ("MSAA");
AddPermutationBool ("VERTEXCOLOR");

AddPermutationConst ("LIGHTMAP", false);
AddPermutationConst ("LIGHTGRID", false);
AddPermutationConst ("VMATERIAL_REFMAP", false);
AddPermutationConst ("VMATERIAL_NORMALMAP", false);
AddPermutationConst ("VMATERIAL_PARALLAX", false);
AddPermutationConst ("LIGHTMAPDOT3", false);
AddPermutationConst ("VMATERIAL_GLOWMAP", false);
AddPermutationConst ("VMATERIAL_PARALLAX_REFINEMENT", false);
AddPermutationConst ("FULLBRIGHT", true);
AddPermutationConst ("DEFERRED_OUTPUT", true);
AddPermutationConst ("VMATERIAL_PARALLAX_DEPTH_DISPLACEMENT", false);


--   Filtering-Function to discard shaders on unsupported platforms:

function IsPermutationSupported (PermutationValues, Platform)

  if (PermutationValues["MSAA"] == true) and (Platform != DX11) then
    return false;
  end

  return true;
end


