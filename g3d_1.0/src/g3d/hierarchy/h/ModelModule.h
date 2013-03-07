#ifndef G3D_HIERARCHY_MODEL_MODULE_H
#define G3D_HIERARCHY_MODEL_MODULE_H

#include "DummyModule.h"
#include "Scene.h"

//a game object module that encapsulates a model

class CG3DModelModule : public CG3DDummyModule
{
public:
	CG3DModelModule(CG3DScene* scene, CG3DMesh* mesh, CG3DMaterial** mats);
	~CG3DModelModule() { delete[] m_SubModels; }

	//handle the set local event
	void SetLocal(CG3DMatrix local);
protected:
	virtual void EnableInternal();
	virtual void DisableInternal();
private:
	CG3DPtr<CG3DPositioner> m_Pos;
	CG3DPtr<CG3DScene> m_Scene;
	int m_NumSubModels;
	CG3DSubModel** m_SubModels;
};

#endif