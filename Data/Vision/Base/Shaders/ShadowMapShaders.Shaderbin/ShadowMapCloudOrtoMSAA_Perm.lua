
-- *** Vision Shader-Permutation Config-File *** 


--   Functions to add Permutation-Variables and their valid Values:

-- AddPermutationBool      ("VAR_NAME");
-- AddPermutationBoolGroup ("VAR_NAME1", "VAR_NAME2", "VAR_NAME3", ...);
-- AddPermutationInt       ("VAR_NAME", int1, int2, int3, ...);
-- AddPermutationConst     ("VAR_NAME", true/false);
-- AddPermutationConst     ("VAR_NAME", int);


--   The following Permutation-Variables were extracted from the Shader:

AddPermutationBool ("RANDOMIZED");
AddPermutationInt ("MAX_CASCADES", 1, 2, 3, 4);
AddPermutationInt ("MSAA_SAMPLES", 2, 4, 8, 16);
AddPermutationBool ("MSAA_SINGLESAMPLE");
AddPermutationBoolGroup ("SHADOWING_MODE_PCF4", "SHADOWING_MODE_PCF8", "SHADOWING_MODE_PCF16", "SHADOWING_MODE_PCSS16", "SHADOWING_MODE_CHS");
AddPermutationConst ("SHADOWING_PROJECTION_ORTHOGRAPHIC", true);
AddPermutationBoolGroup ("SHADOWING_SPLITMODE_INTERVAL", "SHADOWING_SPLITMODE_INTERVAL_DEPTH", "SHADOWING_SPLITMODE_BOUNDINGBOX");
AddPermutationConst ("SHADOWING_LAYERMASK", false);
AddPermutationConst ("SHADOWING_CASCADED", true);
AddPermutationConst ("SHADOWING_SIMPLE_FALLBACK_FOR_FARTHER_CASCADES", false);
AddPermutationConst ("SHADOWING_MODE_PCF32", false);
AddPermutationConst ("SHADOWING_PROJECTION_POINT", false);
AddPermutationConst ("SHADOWING_DEBUG_OUTPUT", false);
AddPermutationConst ("CLOUD_MAPPING", true);


--   Filtering-Function to quickly remove unnecessary permutations:

function IsPermutationSupported (PermutationValues)


  if (PermutationValues["RANDOMIZED"] == true) then
  
    if ((PermutationValues["SHADOWING_MODE_PCF4"] == true) or
        (PermutationValues["SHADOWING_MODE_CHS"] == true) or
        (PermutationValues["SHADOWING_DEBUG_OUTPUT"] == true)) then
      return false;
    end
    
  end

  return true;
end


