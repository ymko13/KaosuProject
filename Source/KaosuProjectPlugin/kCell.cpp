#include "KaosuProjectPluginPCH.h"
#include "kCell.h"

V_IMPLEMENT_SERIAL ( kCell, kBasicEntity, 0, &g_myComponentModule );

START_VAR_TABLE( kCell, kBasicEntity, "The Cell", 0, "")
END_VAR_TABLE

void kCell::InitializeProperties()
{
	kBasicEntity::InitializeProperties();
}

//Place the object, create the objects etc
void kCell::PostInitialize()
{
}

void kCell::EditorThinkFunction()
{
	VString strin;
	strin.Format("< %d, %d >", this->x, this->y);
	Vision::Message.DrawMessage3D(strin, hkvVec3(x * 100.7f + 50.5f, y * 100.7f + 50.5f, 0));
}

void kCell::Dispose()
{
	DisposeObject();
	VString key;
	for(int i = 0; i < 6; i++)
	{
		key.FormatEx("C_%d_%d_Tree_%d", x, y, i);
		VSmartPtr<kStatObject> eCell = (kStatObject *) Vision::Game.SearchEntity(key);
		if(eCell.m_pPtr != NULL)
		{
			eCell->DisposeObject();
		}
	}
	for(int i = 0; i < 10; i++)
	{
		key.FormatEx("C_%d_%d_Obj_%d", x, y, i);
		VSmartPtr<kStatObject> eCell = (kStatObject *) Vision::Game.SearchEntity(key);
		if(eCell.m_pPtr != NULL)
		{
			eCell->DisposeObject();
		}
	}
}

kCell::kCell(void)
{	
}

kCell::~kCell(void)
{
}
