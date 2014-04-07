#pragma once

enum kEffect_Type
{
	ET_Buff = 0,
	ET_Debuff = 1
};
enum kEffect_Buff
{
	EB_HPRegen = 0
};
enum kEffect_Debuff
{
	ED_Poison = 0,
	ED_Freeze = 1,
	ED_Burn = 2,
	ED_Stun = 3
};

class kEffect
{
public:
	kEffect(void);
	~kEffect(void);

	void	SetTimeSpan(const float time = 0.f);
	float	GetTimeSpan() const;
	void	SetValue(const float value = 0.f);
	int		GetIntValue() const;
	float	GetFloatValue() const;

	void	SetType(const kEffect_Type type);
	void	SetBuff(const kEffect_Buff type);
	void	SetDebuff(const kEffect_Debuff type);
	kEffect_Type	GetType() const;
	kEffect_Buff	GetBuff() const;
	kEffect_Debuff	GetDebuff() const;

protected:
	kEffect_Type	m_effect_type;
	kEffect_Buff	m_effect_buff;
	kEffect_Debuff	m_effect_debuff;

	float m_effect_timespan;
	float m_effect_value;

	friend class kCharStats;
};

