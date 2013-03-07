#ifndef G3D_HIERARCHY_UPDATE_MODULE_H
#define G3D_HIERARCHY_UPDATE_MODULE_H

#include "G3DCore.h"
#include "Module.h"
#include "GameObject.h"

//base module for modules that are updated each frame
class CG3DUpdateModule : public CG3DModule {
public:
	CG3DUpdateModule(int priority = 0) : m_Priority(priority) {}
	virtual void DoEvent(CG3DEvent* event) {
		if(event->m_Event == G3D_UPDATE_EVENT)
			DoUpdate(((CG3DUpdateEvent*)event)->m_TimeStep);
	}
	virtual void DoUpdate(float timeStep) = 0;	//override to do update
protected:
	virtual void EnableInternal() {
		m_Parent->AddHandler(G3D_UPDATE_EVENT, this, m_Priority);
	}
	virtual void DisableInternal() {
		m_Parent->RemoveHandler(G3D_UPDATE_EVENT, this);
	}
protected:
	int m_Priority;
};

#endif