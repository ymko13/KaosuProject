
-- *** Vision Shader-Permutation Config-File *** 


--   Functions to add Permutation-Variables and their valid Values:

-- AddPermutationBool      ("VAR_NAME");
-- AddPermutationBoolGroup ("VAR_NAME1", "VAR_NAME2", "VAR_NAME3", ...);
-- AddPermutationInt       ("VAR_NAME", int1, int2, int3, ...);
-- AddPermutationConst     ("VAR_NAME", true/false);
-- AddPermutationConst     ("VAR_NAME", int);

AddPermutationBool("GAMMA_CORRECTION");
AddPermutationBool("GAMMA_CORRECTION_POST_TRANSFORM");

--   Filtering-Function to remove unnecessary permutations:

function IsPermutationSupported (PermutationValues, Platform)

  if (PermutationValues["GAMMA_CORRECTION_POST_TRANSFORM"] and
     (not PermutationValues["GAMMA_CORRECTION"]))
  then
    return false;
  end

  return true;
end


