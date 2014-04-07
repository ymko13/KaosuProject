
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationInt("NUM_3WAY_LAYERS", 0, 1, 2, 3, 4);
AddPermutationConst("MSAA", false);

--   Filtering-Function to quickly remove unnecessary permutations:

function IsPermutationSupported(PermutationValues)
  return true;
end



