#include "KaosuProjectPluginPCH.h"
#include "kController.h"
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

V_IMPLEMENT_SERIAL(kController, kBaseComponent, 0, &g_myComponentModule);

START_VAR_TABLE(kController, kBaseComponent, "Basic Controller", VVARIABLELIST_FLAGS_NONE, "kController")
	DEFINE_VAR_ENUM_AND_NAME(kController, m_new_state, "Current Task", "The current task", "NotControlled",
		"NotControlled/Idling/Moving/AttackingMelee/AttackingRanged/Protecting/Wandering/Fleeing/AskingForHelp/Pursuit", 0, 0);
END_VAR_TABLE

kController::kController(void)
	: kBaseComponent(),
	m_current_state(kControllerState::NotControlled),
	m_new_state(kControllerState::Idling),
	m_call_on_enter(true),
	m_changing_state(false),
	m_direction(0.f)
{
}

void kController::SetOwner(VisTypedEngineObject_cl *pOwner)
{
	kBaseComponent::SetOwner(pOwner);
	if(pOwner!=NULL)
	{ 
		if(pOwner)
		{
			// Create states
			m_states[kControllerState::NotControlled] = new kControllerState::fNotControlled();
			m_states[kControllerState::Moving] = new kControllerState::fMoving();
			m_states[kControllerState::Idling] = new kControllerState::fIdling();
			m_states[kControllerState::Fleeing] = new kControllerState::fFleeing();
			m_states[kControllerState::Wandering] = new kControllerState::fWandering();
			m_call_on_enter = true;
		}
	}
	else
	{

	}
}

void kController::SetState(kControllerState::Enum newState)
{
	VASSERT(!m_changing_state);
	if(!m_changing_state && m_current_state != newState)
	{
		if(m_current_state == kControllerState::Idling && newState == kControllerState::Fleeing)
		{
			m_changing_state = false;
			return;
		}
		
		m_new_state = m_current_state;
		m_changing_state = true;

		kControllerStateBase *currentState = m_states[m_current_state];
		if(currentState)
		{
			currentState->OnExit(this);
		}

		m_current_state = newState;

		currentState = m_states[m_current_state];

		if(currentState && m_call_on_enter)
		{
			currentState->OnEnter(this);
		}

		Vision::GetConsoleManager()->OutputTextLine(currentState->GetName());
		

		m_call_on_enter = true;
		m_changing_state = false;
	}
}

void kController::RestoreState()
{
	m_call_on_enter = false;
	this->SetState(m_new_state);
	m_new_state = kControllerState::Fleeing;
}

kControllerState::Enum kController::GetState()
{
	return m_current_state;
}

void kController::RequestPath(hkvVec3 const& dest)
{
	if (!GetCharacter()) return;
	kCharacter* charC = GetCharacter();

	m_full_path.SetPath(kMap::m_full_path); 
	
	hkvVec3 curPos = charC->GetPosition();
	
	SerialDynArray_cl<hkvVec3> new_path = SerialDynArray_cl<hkvVec3>();

	if(m_current_state == kControllerState::Moving)
	{
		int spIndex, epIndex;
		hkvVec3 spPoint, epPoint;
		m_full_path.GetClosestPoint(spIndex, curPos, spPoint);
		m_full_path.GetClosestPoint(epIndex, dest, epPoint);
		new_path[new_path.GetFreePos()] = dest;
		if(spIndex > epIndex)
		{
			for(int i = epIndex; i <= spIndex; i++)
			{
				new_path[new_path.GetFreePos()] = m_full_path.GetAt(i);
			}
		}
		else
		{
			for(int i = epIndex; i >= spIndex; i--)
			{
				new_path[new_path.GetFreePos()] = m_full_path.GetAt(i);
			}
		}
	}
	else if(m_current_state == kControllerState::Wandering)
	{
		for(int i = 2; i >= 0; i--)
		{
			int x = kMap::getRandom(0, 50) > 25 ? kMap::getRandom(10,110) : -kMap::getRandom(10,110);
			int y = kMap::getRandom(0, 50) > 25 ? kMap::getRandom(10,110) : -kMap::getRandom(10,110);
			hkvVec3 wanderTo = hkvVec3((float)x, (float)y, 0);
			wanderTo.normalizeIfNotZero();
			new_path[new_path.GetFreePos()] = m_wander_position + wanderTo * m_wander_range;
		}
	}

	kAiPath path = kAiPath();
	new_path.AdjustSize();
	path.SetPath(new_path);

	m_current_path = path;
}

void kController::SetDirection(hkvVec3 dir)
{
	m_direction = dir;
	this->GetCharacter()->SetDirection(m_direction);
}

hkvVec3 kController::GetNextPointInPath()
{	
	if (!GetCharacter()) return hkvVec3(0);
	kCharacter* charC = GetCharacter();
	
	hkvVec3 curPos = charC->GetPosition();
	hkvVec3 curPoint = m_current_path.GetCurrentPoint();
	curPos.z = curPoint.z;
	float dist = curPos.getDistanceTo(curPoint);

	hkvVec3 next(0);
	if(dist < GetDistanceToGoalTolerance())
	{
		next = m_current_path.GetNextPoint();

		if(next.isIdentical(curPoint))
		{
			if(m_current_state == kControllerState::Moving)
				this->SetState(kControllerState::Wandering);
			else if(m_current_state == kControllerState::Wandering)
				this->SetState(kControllerState::Moving);
		}
		else if(m_current_state == kControllerState::Wandering && m_new_state != kControllerState::Idling)
			this->SetState(kControllerState::Idling);

	}
	else
	{
		next = curPoint;
	}

	return next;
}

float kController::GetDistanceToGoalTolerance() const
{
	return 15.f;
}

void kController::SetWandering(hkvVec3 startPos, float wanderRange)
{
	m_wander_position = startPos;
	m_wander_range = wanderRange;
}

float kController::GetWantedSpeed()
{
	if (!GetCharacter()) return 0.f;
	kCharacter* charC = GetCharacter();
	return (float)charC->GetCharacterStats().GetMovementSpeed();
}

void kController::onFrameUpdate(float delta)
{
	kBaseComponent::onFrameUpdate(delta);

	if(m_current_state == kControllerState::NotControlled)
	{
		this->SetState(kControllerState::Moving);
	}

	if(m_states[m_current_state])
	{
		m_states[m_current_state]->OnTick(this, delta);
	}
}

bool kController::TryMeleeAttack()
{
	return true;
}
bool kController::TryRangedAttack()
{
	return false;
}