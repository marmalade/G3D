#ifndef G3D_HIERARCHY_SCENE_H
#define G3D_HIERARCHY_SCENE_H

#include "G3DCore.h"
#include "SubModel.h"

#include <map>

//a scene is a collection of modules drawn in a particular order

//this is the base class for all scenes
class CG3DScene : public CG3DResource
{
public:
	typedef void (CG3DResource::* SceneFunction)();
public:
	CG3DScene() { SetInternalFlag(DYNAMIC, true); }

	//add and remove sets of submodels
	void Add(CG3DSubModel** subModels, int num);
	void Remove(CG3DSubModel** subModels, int num);

	//set the camera
	virtual void Set(CG3DCamera* camera) = 0;

	//add or remove a sub model
	virtual void Add(CG3DSubModel* subModel) = 0;
	virtual void Remove(CG3DSubModel* subModel) = 0;

	//override to apply resource functions to all children
	virtual void Traverse(SceneFunction function) = 0;
	virtual void Apply() = 0;	//draw the scene
protected:
	virtual void UploadInternal();
	virtual void ReloadInternal();
	virtual void UnloadInternal();
	virtual void ApplyInternal() {}
public:
	virtual void ReleaseInternal();
};

//this scene sorts per material
class CG3DSimpleScene : public CG3DScene
{
	typedef std::map<CG3DMaterial*, std::vector<CG3DSubModel*> > Map;
public:
	CG3DSimpleScene() : m_CurrPos(NULL), m_Camera(NULL) {}
	virtual void Add(CG3DSubModel* subModel);
	virtual void Remove(CG3DSubModel* subModel);
	virtual void Set(CG3DCamera* camera);

	virtual void Traverse(SceneFunction function);
	virtual void Apply();
private:
	Map m_Map;
	CG3DPositioner* m_CurrPos;	//not CG3DPtr's as they get released by the resource functions
	CG3DCamera* m_Camera;
};

#endif