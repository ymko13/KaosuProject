#pragma once
#include <Vision/Runtime/Engine/System/Vision.hpp>
#include "KaosuProjectPluginModule.h"


class kBasicEntity : public VisBaseEntity_cl
{
public:
	kBasicEntity(void);
	~kBasicEntity(void);

	virtual void InitializeProperties();
	virtual void PostInitialize();
	virtual void MoveTo(const hkvVec3 position);
	virtual void MoveBy(const hkvVec3 position);
protected:
	void Serialize(VArchive& ar) HKV_OVERRIDE;

	VBool WantsDeserializationCallback(const VSerializationContext& context) HKV_OVERRIDE;
	void OnDeserializationCallback(const VSerializationContext& context) HKV_OVERRIDE;
private:
	V_DECLARE_SERIAL_DLLEXP(kBasicEntity, SAMPLEPLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(kBasicEntity, SAMPLEPLUGIN_IMPEXP);
};


