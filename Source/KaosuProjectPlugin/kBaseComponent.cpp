#include "KaosuProjectPluginPCH.h"
#include "kBaseComponent.h"

V_IMPLEMENT_SERIAL(kBaseComponent, IVObjectComponent, 0, &g_myComponentModule);

kBaseComponent_ComponentManager kBaseComponent_ComponentManager::g_GlobalManager;

kBaseComponent::kBaseComponent(void)
{
}

kBaseComponent::~kBaseComponent(void)
{
}

void kBaseComponent::Serialize(VArchive &ar)
{
	IVObjectComponent::Serialize(ar);
	if(ar.IsLoading())
	{
		kSerializer::ReadVariableList(this, ar);
	}
	else
	{
		kSerializer::WriteVariableList(this, ar);
	}
}

void kBaseComponent::SetOwner(VisTypedEngineObject_cl *pOwner)
{
  IVObjectComponent::SetOwner( pOwner );
  if (pOwner!=NULL)
  {
    kBaseComponent_ComponentManager::GlobalManager().Instances().AddUnique(this);
    Vision::Message.Add("kBaseComponent - Component Instance created");
  }
  else
  {
    Vision::Message.Add("kBaseComponent - Removed Component Instance from component Manager");
    kBaseComponent_ComponentManager::GlobalManager().Instances().SafeRemove(this);    
  }
}

void kBaseComponent::onFrameUpdate(float delta)
{
	if (!GetOwner())return;

	kCharacter* charC = (kCharacter *)GetOwner();
	
	hkvVec3 vPos = charC->GetPosition();
	VString s;
	int health = charC->GetCharacterStats().GetHealth(),
		healthmax = charC->GetCharacterStats().GetHealthMax(),
		healthpercent = charC->GetCharacterStats().GetHealthPercentage(),
		movement = charC->GetCharacterStats().GetMovementSpeed();
	s.FormatEx("HPoints: %d/%d(%d)  MSpeed: %d", health, healthmax, healthpercent, movement);
	Vision::Message.DrawMessage3D(s,vPos);
}

START_VAR_TABLE(kBaseComponent, IVObjectComponent, "Base Component", VVARIABLELIST_FLAGS_NONE, "kBaseComponent")

END_VAR_TABLE


