#include "KaosuProjectPluginPCH.h"
#include "FreezeMe.h"


V_IMPLEMENT_ACTION( "FreezeMe", FreezeMe, VAction,&g_myComponentModule, NULL )

FreezeMe::FreezeMe(){}
  
FreezeMe::~FreezeMe(){}

VBool FreezeMe::Do( const class VArgList &argList )
{    
	kCharacter* charac = (kCharacter *) Vision::Game.SearchEntity("Player");
	VASSERT(charac);
	kEffect e = kEffect();
	e.SetType(ET_Debuff);
	e.SetDebuff(ED_Freeze);
	e.SetTimeSpan(7.35f);
	e.SetValue(0.7f);
	charac->ApplyEffect(e);
	return TRUE;
}
