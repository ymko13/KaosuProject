#pragma once
#include "KaosuProjectPluginModule.h"
#include "kBasicEntity.h"
#include <Vision/Runtime/Base/String/VString.hpp> 

class kCell : public kBasicEntity
{
public:	
	kCell(void);
	~kCell(void);  

	int x;
	int y;

	void InitializeProperties() HKV_OVERRIDE;
	void PostInitialize() HKV_OVERRIDE;
	
	void EditorThinkFunction() HKV_OVERRIDE;
	void Dispose();
private:	
	V_DECLARE_SERIAL_DLLEXP ( kCell, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE ( kCell, SAMPLEPLUGIN_IMPEXP );
};
