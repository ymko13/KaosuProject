#include "KaosuProjectPluginPCH.h"
#include "StunMe.h"

V_IMPLEMENT_ACTION( "StunMe", StunMe, VAction,&g_myComponentModule, NULL )

StunMe::StunMe(){}
  
StunMe::~StunMe(){}

VBool StunMe::Do( const class VArgList &argList )
{    
	kCharacter* charac = (kCharacter *) Vision::Game.SearchEntity("Player");
	VASSERT(charac);
	kEffect e = kEffect();
	e.SetType(ET_Debuff);
	e.SetDebuff(ED_Stun);
	e.SetTimeSpan(1.5f);
	e.SetValue(0.f);
	charac->ApplyEffect(e);
	return TRUE;
}