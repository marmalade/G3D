#ifndef G3D_HIERARCHY_CAMERA_MODULE_H
#define G3D_HIERARCHY_CAMERA_MODULE_H

#include "DummyModule.h"
#include "Scene.h"

//a game object modules encapsulating a camera

class CG3DCameraModule : public CG3DDummyModule
{
public:
	CG3DCameraModule(CG3DScene* scene, CG3DEffect* effect, CG3DMatrix proj) : m_Scene(scene), m_Camera(new CG3DCamera(effect)) { m_Camera->SetProj(proj); }

	//set the view matrix
	virtual void SetLocal(CG3DMatrix local);
protected:
	virtual void EnableInternal();
	virtual void DisableInternal();
public:
	CG3DPtr<CG3DScene> m_Scene;
	CG3DPtr<CG3DCamera> m_Camera;
};

#endif