#include "KaosuProjectPluginPCH.h"
#include "kEffect.h"

kEffect::kEffect(void)
	: m_effect_type(ET_Buff),
	m_effect_buff(),
	m_effect_debuff(),
	m_effect_timespan(1.f),
	m_effect_value(0.f)
{
}

kEffect::~kEffect(void)
{
}

void kEffect::SetTimeSpan(const float time)
{
	m_effect_timespan = time;
}
float kEffect::GetTimeSpan() const
{
	return m_effect_timespan;
}

void kEffect::SetValue(const float value)
{
	m_effect_value = value;
}
int kEffect::GetIntValue() const
{
	return (int)(m_effect_value * 100);
}
float kEffect::GetFloatValue() const
{
	return m_effect_value;
}

void kEffect::SetType(const kEffect_Type type)
{
	m_effect_type = type;
}
void kEffect::SetBuff(const kEffect_Buff type)
{
	m_effect_buff = type;
}
void kEffect::SetDebuff(const kEffect_Debuff type)
{
	m_effect_debuff = type;
}

kEffect_Type kEffect::GetType() const
{
	return m_effect_type;
}
kEffect_Buff kEffect::GetBuff() const
{
	return m_effect_buff;
}
kEffect_Debuff kEffect::GetDebuff() const
{
	return m_effect_debuff;
}


