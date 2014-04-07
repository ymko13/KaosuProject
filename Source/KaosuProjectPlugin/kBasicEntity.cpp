#include "kBasicEntity.h"
#include "KaosuProjectPluginPCH.h"

V_IMPLEMENT_SERIAL ( kBasicEntity, VisBaseEntity_cl, 0, &g_myComponentModule );

START_VAR_TABLE( kBasicEntity, VisBaseEntity_cl, "Basic Entity of the game", 0, "")
END_VAR_TABLE

//Code for serialize is more or less copied from the
//RPG Game Plugin(BaseEntity) as an example of how Serialize should be done
void kBasicEntity::Serialize(VArchive &ar)
{
	VisBaseEntity_cl::Serialize(ar);
	if(ar.IsLoading())
	{
		kSerializer::ReadVariableList(this, ar);
	}
	else
	{
		kSerializer::WriteVariableList(this, ar);
	}
}

void kBasicEntity::InitializeProperties()
{
	TriggerScriptEvent("OnInitProperties");
}

void kBasicEntity::PostInitialize()
{
}

void kBasicEntity::MoveTo(const hkvVec3 pos)
{
	this->SetPosition(pos);
}

void kBasicEntity::MoveBy(const hkvVec3 pos)
{
	hkvVec3 p = this->GetPosition();
	p += pos;
	this->SetPosition(p);
}

void kBasicEntity::OnDeserializationCallback(const VSerializationContext& cnt)
{
	VisBaseEntity_cl::OnDeserializationCallback(cnt);

	InitializeProperties();

	PostInitialize();
}

VBool kBasicEntity::WantsDeserializationCallback(const VSerializationContext& cnt)
{
	return TRUE;
}

kBasicEntity::kBasicEntity(void)
{
}

kBasicEntity::~kBasicEntity(void)
{
}