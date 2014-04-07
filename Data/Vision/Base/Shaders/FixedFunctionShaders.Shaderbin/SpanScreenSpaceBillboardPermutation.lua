
-- *** Vision Shader-Permutation Config-File *** 


--   Functions to add Permutation-Variables and their valid Values:

-- AddPermutationBool      ("VAR_NAME");
-- AddPermutationBoolGroup ("VAR_NAME1", "VAR_NAME2", "VAR_NAME3", ...);
-- AddPermutationInt       ("VAR_NAME", int1, int2, int3, ...);
-- AddPermutationConst     ("VAR_NAME", true/false);
-- AddPermutationConst     ("VAR_NAME", int);


--   The following Permutation-Variables were extracted from the Shader:

AddPermutationInt ("MRT", 1, 2, 3, 4);


--   Filtering-Function to remove unnecessary permutations:

function IsPermutationSupported (PermutationValues, Platform)


if ((PermutationValues["MRT"] > 1) and (Platform == PSP2)) then
  return false;
end

if ((PermutationValues["MRT"] > 1) and (Platform == GLES2)) then
  return false;
end

  return true;
end