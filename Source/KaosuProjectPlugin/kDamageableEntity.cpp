#include "KaosuProjectPluginPCH.h"
#include "kDamageableEntity.h"

V_IMPLEMENT_SERIAL ( kDamageableEntity, kBasicEntity, 0, &g_myComponentModule );

START_VAR_TABLE( kDamageableEntity, kBasicEntity, "This is a damageable entity", 0, "")
  DEFINE_VAR_FLOAT_AND_NAME(kDamageableEntity, m_col_radius, "Collision Radius", "Collision shape radius", "35", 0, 0);
  DEFINE_VAR_FLOAT_AND_NAME(kDamageableEntity, m_col_height, "Collision Height", "Collision shape height", "70", 0, 0);
END_VAR_TABLE

kDamageableEntity::kDamageableEntity()
	: kBasicEntity()
	, m_col_radius(100.f)
	, m_col_height(100.f)
{

}

kDamageableEntity::~kDamageableEntity(void)
{
}

int kDamageableEntity::TakeDamage(const int damageAmount, const mStats_AttackType attack)
{
  return 0;
}

float kDamageableEntity::GetColRadius() const
{
  return m_col_radius;
}

float kDamageableEntity::GetColHeight() const
{
  return m_col_height;
}