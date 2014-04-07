#pragma once
#include "KaosuProjectPluginModule.h"
#include "kBasicEntity.h"

class kBasicEntity;
class kStatObject;

class kStatObject : public kBasicEntity
{
public:
	kStatObject(void);
	~kStatObject(void);

	int x;
	int y;
	int objId;

	void InitializeProperties() HKV_OVERRIDE;
	void PostInitialize() HKV_OVERRIDE;
	void Set(int x, int y, int objId);
private:	
	V_DECLARE_SERIAL_DLLEXP ( kStatObject, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE ( kStatObject, SAMPLEPLUGIN_IMPEXP );
};
