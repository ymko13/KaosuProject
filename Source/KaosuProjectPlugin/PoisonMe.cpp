#include "KaosuProjectPluginPCH.h"
#include "PoisonMe.h"

V_IMPLEMENT_ACTION( "PoisonMe", PoisonMe, VAction,&g_myComponentModule, NULL )

PoisonMe::PoisonMe(){}
  
PoisonMe::~PoisonMe(){}

VBool PoisonMe::Do( const class VArgList &argList )
{    
	kCharacter* charac = (kCharacter *) Vision::Game.SearchEntity("Player");
	VASSERT(charac);
	kEffect e = kEffect();
	e.SetType(ET_Debuff);
	e.SetDebuff(ED_Poison);
	e.SetTimeSpan(6.15f);
	e.SetValue(0.7f);
	charac->ApplyEffect(e);
	return TRUE;
}
