#pragma once
#include "kEffect.h"

/*
Hero < Heavy < Normal
*/

enum mStats_AttackType
{
	AtT_Normal = 0,
	AtT_Heavy = 1,
	AtT_Hero = 2
};

enum mStats_ArmorType
{
	ArT_Normal = 0,
	ArT_Heavy = 1,
	ArT_Hero = 2
};

class kCharStats
{
public:
	kCharStats(void);
	~kCharStats(void);

	/*Char Attack Stats*/
	void	SetAttackPower(int const attackpower = 1);
	int		GetAttackPower() const;
	void	SetAttackSpeed(float const attackspeed = 1);
	float	GetAttackSpeed() const;
	void	SetCriticalChance(float const critchance = 0.f);
	float	GetCriticalChance() const;
	
	/*Char Health Stats*/
	void	SetHealth(int const health = 0);
	int		GetHealth() const;
	int		GetHealthPercentage() const;
	void	SetHealthMax(int const health = 0);
	int		GetHealthMax() const;
	void	SetHealthRegeneration(VBool const enabled = 0);
	void	SetHealthRegenerationRate(float const healthRegenRate = 0);

	/*Char Armor Stats*/
	//100 armor is 100% defense(75 armor is a cap)
	void	SetArmor(int const armor = 0);
	int		GetArmor() const;	

	void	SetAttackType(mStats_AttackType const attacktype = AtT_Normal);
	void	SetArmorType(mStats_ArmorType const armortype = ArT_Normal);
	
	mStats_AttackType	GetAttackType() const;
	mStats_ArmorType	GetArmorType() const;

	/*Char Movement Speed Stats*/
	void	SetMovementSpeed(int const movementspeed = 50);
	int		GetMovementSpeed() const;

	/*Char Apply Effects*/
	//Damage STACKS and time RENEWS
	void	SetHealthDegeneration(float const timeLapse, float const healthDegenRate);
	//The debuff renews, speed increases overtime
	void	SetImpairMovementSpeed(float const timeLapse, float const degenPercentage);
	//The debuff can't happen twice, so no stun locks happen
	void	SetStunned(float const timeLapse);
	bool	GetStunned();
	void	ApplyEffect(kEffect effect);

	/*Char Stats Updates*/
	void	UpdateStats(float const delta);
	void	UpdateHealthRegen(float const delta);
	void	UpdateHealthDegen(float const delta);
	void	UpdateMovementSpeed(float const delta);
	void	UpdateStunned(float const delta);

	static const int CalculateRealDamage(int const rawdamage, mStats_AttackType const attackerAttack, int const rawarmor, mStats_ArmorType const attackArmor);

protected:
	/*Char Attack Stats*/
	int m_attack_power;
	float m_attack_speed;
	float m_critical_chance;
	
	/*Char Health Stats*/
	bool m_dead;
	int m_health;
	int m_health_max;
	VBool m_health_regen;
	float m_health_regen_rate;
	float m_health_regen_acum;

	/*Char Armor Stats*/
	int m_armor;
	mStats_ArmorType m_armor_type;
	mStats_AttackType m_attack_type;

	/*Char Movement Speed Stats*/
	int m_movement_speed;
	int m_movement_speed_normal;

	/*Char Effects*/
	float m_movement_speed_impair_tl; //time left
	float m_movement_speed_impair_tl_total; //total time
	float m_movement_speed_normal_pl; //percent lost
	float m_health_degen_tl;
	float m_health_degen_rate;
	float m_health_degen_acum;
	bool m_stunned;
	float m_stunned_tl;

	friend class kCharacter;
};

