#include "KaosuProjectPluginPCH.h"
#include "FearMe.h"

V_IMPLEMENT_ACTION( "FearMe", FearMe, VAction,&g_myComponentModule, NULL )
FearMe::FearMe(){}
FearMe::~FearMe(){}
VBool FearMe::Do( const class VArgList &argList )
{    
	kCharacter* charac = (kCharacter *) Vision::Game.SearchEntity("Player");
	VASSERT(charac);
	kController* control = static_cast<kController*>(charac->Components().GetComponentOfBaseType(V_RUNTIME_CLASS(kController)));
	VASSERT(control);
	control->SetState(kControllerState::Fleeing);
	return TRUE;
}
