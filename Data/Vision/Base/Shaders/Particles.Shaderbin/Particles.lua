-- Particles.lua
-- *** Vision Shader-Permutation Config-File *** 

AddPermutationBool ("HWSPANNING");
AddPermutationBool ("SMOOTHANIM");
AddPermutationBool ("ALPHATESTINSHADER");

function IsPermutationSupported (PermutationValues, Platform)
  
  -- alpha test instruction in shader is only needed on some platforms
  if (PermutationValues["ALPHATESTINSHADER"] and
      (Platform == DX9 or Platform == XBOX360 or
      Platform == PS3 or Platform == PSP2)) 
  then
    return false;
  end

  return true;
end
