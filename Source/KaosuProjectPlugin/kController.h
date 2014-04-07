#pragma once
#include "KaosuProjectPluginPCH.h"

class kCharacter;
class kDamageableEntity;

class kControllerStateBase;


namespace kControllerState
{
	enum Enum
	{
		NotControlled,
		Idling,
		Moving,
		AttackingMelee,
		AttackingRanged,

		Protecting,
		Wandering,
		Fleeing,
		AskingForHelp,
		Pursuit,

		NumStates
	};
}

class kController : public kBaseComponent
{
public:
	kController(void);

	void SetState(kControllerState::Enum newState);
	void RestoreState();
	kControllerState::Enum GetState();
	kCharacter* GetCharacter() const { 
		return vstatic_cast<kCharacter*>(GetOwner());
	}

	//AI
	void RequestPath(hkvVec3 const& dest);

	VOVERRIDE hkvVec3 GetNextPointInPath();

	float GetDistanceToGoalTolerance() const;

	void SetDirection(hkvVec3 dir);
	hkvVec3 GetDirection() const {
		return m_speed > 0.f ? m_direction : GetCharacter()->GetDirection();
	}

	float GetSpeed() const {
		return m_speed;
	}

	void SetWandering(hkvVec3 startPos, float wanderRange);

	float GetWantedSpeed();

	VOVERRIDE bool TryMeleeAttack();
	VOVERRIDE bool TryRangedAttack();

	void onFrameUpdate(float delta) HKV_OVERRIDE;

protected:
	void SetOwner(VisTypedEngineObject_cl *owner) HKV_OVERRIDE;
	VSmartPtr<kControllerStateBase> m_states[kControllerState::NumStates];

	bool m_changing_state;
	bool m_call_on_enter;
	kControllerState::Enum m_current_state;
	kControllerState::Enum m_new_state;

	kAiPath m_full_path;
	kAiPath m_current_path;

	float m_speed; 
	hkvVec3 m_direction;
	hkvVec3 m_wander_position;

	float m_wander_range;

private:
	V_DECLARE_SERIAL_DLLEXP( kController, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE( kController, SAMPLEPLUGIN_IMPEXP );
};


class kControllerStateBase : public VRefCounter
{
public:
	VOVERRIDE void OnEnter(kController *controller) {}
	VOVERRIDE void OnExit(kController *controller) {}
	VOVERRIDE void OnTick(kController *controller, float delta) {}

	VOVERRIDE char const *GetName() const = 0;
};

namespace kControllerState
{
	class fNotControlled : public kControllerStateBase
	{
		char const *GetName() const HKV_OVERRIDE { return "Controller::NotControlled"; }
	};	
	class fIdling : public kControllerStateBase
	{
	public:
		float idleFor;
		void OnEnter(kController *controller) HKV_OVERRIDE {
			this->idleFor = (float)kMap::getRandom(50,100) / 100.f;
		}
		void OnExit(kController *controller) HKV_OVERRIDE {
		}
		void OnTick(kController *controller, float delta) HKV_OVERRIDE {
			idleFor -= delta;
			if(idleFor <= 0.f)
				controller->RestoreState();
		}
		char const *GetName() const HKV_OVERRIDE { return "aiController::Idling"; }
	};
	class fFleeing : public kControllerStateBase
	{
	public:
		float fearFor;
		void OnEnter(kController *controller) HKV_OVERRIDE {
			this->fearFor = (float)kMap::getRandom(50,150) / 100.f;
		}
		void OnExit(kController *controller) HKV_OVERRIDE {
		}
		void OnTick(kController *controller, float delta) HKV_OVERRIDE {
			if(!controller->GetCharacter()) return;

			fearFor -= delta;

			(controller->GetCharacter())->MoveBy(-controller->GetDirection() *
				controller->GetWantedSpeed() * delta);

			if(fearFor <= 0.f)
				controller->RestoreState();
		}
		char const *GetName() const HKV_OVERRIDE { return "aiController::Fleeing"; }
	};
	class fMoving : public kControllerStateBase
	{
		void OnEnter(kController *controller) HKV_OVERRIDE {
			controller->RequestPath(kMap::getRandomPoint(100.7f));
		}
		void OnTick(kController *controller, float delta) HKV_OVERRIDE {
			if (!controller->GetCharacter()) return;
			hkvVec3 currentPos = controller->GetCharacter()->GetPosition();
			hkvVec3 resultDir, currentDir, desiredDir;
			desiredDir = controller->GetNextPointInPath() - currentPos;
			desiredDir.normalizeIfNotZero();
			currentDir = controller->GetDirection();
			if(currentDir.dot(desiredDir) < 0.99f)
			{
				float dot = currentDir.dot(desiredDir);
				float theta = hkvMath::acosRad(dot) * 0.15f;
				hkvVec3 vec = (desiredDir - currentDir * dot).getNormalized();
				resultDir = currentDir * hkvMath::cosRad(theta) +
					vec * hkvMath::sinRad(theta);
			}
			else
				resultDir = desiredDir;
			resultDir.z = 0.f;
			hkvVec3 Velocity = resultDir * controller->GetWantedSpeed() * delta;
			controller->SetDirection(resultDir);
			hkvVec3 newPos = currentPos + Velocity;
			controller->GetCharacter()->MoveBy(Velocity);
		}
		char const *GetName() const HKV_OVERRIDE { return "aiController::Moving"; }
	};
	class fWandering : public kControllerStateBase
	{
		void OnEnter(kController *controller) HKV_OVERRIDE {
			kCharacter* chr = controller->GetCharacter();
			controller->SetWandering(chr->GetPosition(), 275.f);
			controller->RequestPath(kMap::m_full_path[kMap::m_full_path.GetValidSize() - 1]);
		}
		void OnTick(kController *controller, float delta) HKV_OVERRIDE {
			if (!controller->GetCharacter()) return;
			hkvVec3 currentPos = controller->GetCharacter()->GetPosition();
			hkvVec3 resultDir, currentDir, desiredDir;
			desiredDir = controller->GetNextPointInPath() - currentPos;
			desiredDir.normalizeIfNotZero();
			currentDir = controller->GetDirection();
			if(currentDir.dot(desiredDir) < 0.99f)
			{
				float dot = currentDir.dot(desiredDir);
				float theta = hkvMath::acosRad(dot) * 0.15f;
				hkvVec3 vec = (desiredDir - currentDir * dot).getNormalized();
				resultDir = currentDir * hkvMath::cosRad(theta) + vec * hkvMath::sinRad(theta);
			}
			else
				resultDir = desiredDir;
			resultDir.z = 0.f;
			hkvVec3 Velocity = resultDir * controller->GetWantedSpeed() * delta;
			controller->SetDirection(resultDir);
			hkvVec3 newPos = currentPos + Velocity;
			controller->GetCharacter()->MoveBy(Velocity);
		}
		char const *GetName() const HKV_OVERRIDE { return "aiController::Wandering"; }
	};
}

