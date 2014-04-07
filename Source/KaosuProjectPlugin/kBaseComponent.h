#pragma once
#include "KaosuProjectPluginPCH.h"

class kBaseComponent : public IVObjectComponent
{
public:
	kBaseComponent(void);
	~kBaseComponent(void);

	VOVERRIDE BOOL CanAttachToObject( VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut )
	{
		if ( !IVObjectComponent::CanAttachToObject( pObject, sErrorMsgOut )) return FALSE;  

		if ( !pObject->IsOfType( V_RUNTIME_CLASS(kBasicEntity) ) )
		{
			sErrorMsgOut = "Component can only be added to instances of mBasicEntity or derived classes.";
			return FALSE;
		}
		return TRUE;
	}
	
	SAMPLEPLUGIN_IMPEXP VOVERRIDE void onFrameUpdate(float delta);

protected:
	void Serialize(VArchive& ar) HKV_OVERRIDE;
	void SetOwner(VisTypedEngineObject_cl *owner) HKV_OVERRIDE;

private:
	V_DECLARE_SERIAL_DLLEXP( kBaseComponent, SAMPLEPLUGIN_IMPEXP );
	V_DECLARE_VARTABLE( kBaseComponent, SAMPLEPLUGIN_IMPEXP );
};

class kBaseComponent_Collection : public VRefCountedCollection<kBaseComponent> {};

//Copied from MyComponent.h
class kBaseComponent_ComponentManager : public IVisCallbackHandler_cl
{
public:
  //   Gets the singleton of the manager
  static kBaseComponent_ComponentManager &GlobalManager(){  return g_GlobalManager;  }
  //   Should be called at plugin initialization time.
  void OneTimeInit(){  Vision::Callbacks.OnUpdateSceneFinished += this;} // listen to this callback
  //   Should be called at plugin de-initialization time
  void OneTimeDeInit(){ Vision::Callbacks.OnUpdateSceneFinished -= this;} // de-register
  //   Callback method that takes care of updating the managed instances each frame
  VOVERRIDE void OnHandleCallback( IVisCallbackDataObject_cl *pData )
  {
    VASSERT( pData->m_pSender==&Vision::Callbacks.OnUpdateSceneFinished );
    // call update function on every component
    const int iCount = m_Components.Count();
    for (int i=0;i<iCount;i++)
		m_Components.GetAt(i)->onFrameUpdate(Vision::GetTimer()->GetTimeDifference());
  }
  //   Gets all VPlayableCharacterComponent instances this manager holds
  inline kBaseComponent_Collection &Instances() { return m_Components; }
protected:
  /// Holds the collection of all instances of MyComponent
  kBaseComponent_Collection m_Components;
  /// One global instance of our manager
  static kBaseComponent_ComponentManager g_GlobalManager;
};

