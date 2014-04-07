#pragma once
#include "KaosuProjectPluginPCH.h"
#include "kBasicEntity.h"

class kDamageableEntity : public kBasicEntity
{
public:
	kDamageableEntity();
	~kDamageableEntity(void);
	
	virtual int TakeDamage(const int damageAmount, const mStats_AttackType attack);

	virtual float GetColRadius() const;
	virtual float GetColHeight() const;
	float m_col_radius;
	float m_col_height;

private:
	V_DECLARE_SERIAL_DLLEXP(kDamageableEntity, SAMPLEPLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(kDamageableEntity, SAMPLEPLUGIN_IMPEXP);
};

