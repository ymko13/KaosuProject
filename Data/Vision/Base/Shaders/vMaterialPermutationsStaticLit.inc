
#include <shaders/vMaterialPermutations.inc>

$ifdef USE_RIMLIGHT
  #define USE_RIMLIGHT
$endif

$ifdef VMATERIAL_CUBE_REFLECTION
  #define VMATERIAL_CUBE_REFLECTION
$endif

$ifdef VMATERIAL_GLOWMAP
  #define VMATERIAL_GLOWMAP
$endif

$ifdef VMATERIAL_GLOWMAP_ADDITIVE
  #define VMATERIAL_GLOWMAP_ADDITIVE
$endif
