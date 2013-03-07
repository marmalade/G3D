#ifndef G3D_HIERARCHY_MODULE_H
#define G3D_HIERARCHY_MODULE_H

#include "G3DCore.h"
#include "Event.h"

//base class for game object modules
//a module can have data and respond to events
//a module can be enabled and disabled

class CG3DGameObject;

class CG3DModule : public CG3DRefCount {
public:
	CG3DGameObject* m_Parent;
private:
	bool m_Enabled;
public:
	CG3DModule() : m_Parent(NULL), m_Enabled(NULL) {}

	//turn the module on and off
	void Enable();
	void Disable();
	bool IsEnabled() { return m_Enabled; }

	virtual void DoEvent(CG3DEvent* event) {}	//override to handle incoming events
	virtual void Release();
	template<class T> T* GetData(const char* name) { return (T*)GetDataInternal(name); }
protected:
	virtual void* GetDataInternal(const char* name) { return NULL; }	//override to handle data fetches
	virtual void EnableInternal() {}	//override to register on event handlers etc
	virtual void DisableInternal() {}	//override to unregister on event handlers etc
};

#endif