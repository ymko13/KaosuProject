
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("RANDOMIZED");
AddPermutationConst ("MAX_CASCADES", 4);
AddPermutationInt ("MSAA_SAMPLES", 2, 4, 8, 16);
AddPermutationBool ("MSAA_SINGLESAMPLE");
AddPermutationBoolGroup ("SHADOWING_MODE_PCF4", "SHADOWING_MODE_PCF8", "SHADOWING_MODE_PCF16", "SHADOWING_MODE_PCSS16", "SHADOWING_MODE_CHS");
AddPermutationConst ("SHADOWING_PROJECTION_ORTHOGRAPHIC", false);
AddPermutationConst ("SHADOWING_SPLITMODE_INTERVAL", false);
AddPermutationConst ("SHADOWING_SPLITMODE_INTERVAL_DEPTH", false);
AddPermutationConst ("SHADOWING_SPLITMODE_BOUNDINGBOX", false);
AddPermutationConst ("SHADOWING_LAYERMASK", false);
AddPermutationConst ("SHADOWING_CASCADED", false);
AddPermutationConst ("SHADOWING_SIMPLE_FALLBACK_FOR_FARTHER_CASCADES", false);
AddPermutationConst ("SHADOWING_MODE_PCF32", false);
AddPermutationConst ("SHADOWING_PROJECTION_POINT", true);
AddPermutationConst ("SHADOWING_DEBUG_OUTPUT", false);
AddPermutationConst ("CLOUD_MAPPING", false);

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


