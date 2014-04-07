
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationConst ("MSAA_SAMPLES", 1);
AddPermutationInt ("MAX_CASCADES", 1, 2, 3, 4);
AddPermutationConst ("MSAA_SINGLESAMPLE", false);
AddPermutationConst ("SHADOWING_MODE_PCF4", true);
AddPermutationConst ("SHADOWING_MODE_PCF8", false);
AddPermutationConst ("SHADOWING_MODE_PCF16", false);
AddPermutationConst ("SHADOWING_MODE_PCSS16", false);
AddPermutationConst ("RANDOMIZED", false);
AddPermutationConst ("SHADOWING_MODE_CHS", false);
AddPermutationBoolGroup ("SHADOWING_SPLITMODE_INTERVAL", "SHADOWING_SPLITMODE_INTERVAL_DEPTH", "SHADOWING_SPLITMODE_BOUNDINGBOX");
AddPermutationConst ("SHADOWING_LAYERMASK", false);
AddPermutationConst ("SHADOWING_CASCADED", true);
AddPermutationConst ("SHADOWING_SIMPLE_FALLBACK_FOR_FARTHER_CASCADES", false);
AddPermutationConst ("SHADOWING_DEBUG_OUTPUT", true);
AddPermutationConst ("SHADOWING_MODE_PCF32", false);
AddPermutationConst ("SHADOWING_PROJECTION_ORTHOGRAPHIC", true);
AddPermutationConst ("SHADOWING_PROJECTION_POINT", false);
AddPermutationConst ("CLOUD_MAPPING", false);

