#ifndef G3D_HIERARCHY_DUMMY_MODULE_H
#define G3D_HIERARCHY_DUMMY_MODULE_H

#include "G3DCore.h"
#include "G3DHelper.h"
#include "Module.h"
#include "GameObject.h"

//a game object module that sets the world matrix from a local matrix passed via G3D_SET_LOCAL_EVENT and the parent world matrix

class CG3DDummyModule : public CG3DModule
{
public:
	virtual void DoEvent(CG3DEvent* event);

	//dal with the G3D_SET_LOCAL_EVENT
	virtual void SetLocal(CG3DMatrix local);
protected:
	virtual void EnableInternal() {
		m_Parent->AddHandler(G3D_SET_LOCAL_EVENT, this);
	}
	virtual void DisableInternal() {
		m_Parent->RemoveHandler(G3D_SET_LOCAL_EVENT, this);
	}
};

#endif