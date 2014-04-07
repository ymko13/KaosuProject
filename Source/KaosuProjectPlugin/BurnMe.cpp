#include "KaosuProjectPluginPCH.h"
#include "BurnMe.h"

V_IMPLEMENT_ACTION( "BurnMe", BurnMe, VAction,&g_myComponentModule, NULL )

BurnMe::BurnMe(){}
  
BurnMe::~BurnMe(){}

VBool BurnMe::Do( const class VArgList &argList )
{    
	kCharacter* charac = (kCharacter *) Vision::Game.SearchEntity("Player");
	VASSERT(charac);
	kEffect e = kEffect();
	e.SetType(ET_Debuff);
	e.SetDebuff(ED_Burn);
	e.SetTimeSpan(4.25f);
	e.SetValue(0.15f);
	charac->ApplyEffect(e);
	return TRUE;
}
