#ifndef G3D_HELPER_MODEL_H
#define G3D_HELPER_MODEL_H

#include "G3DCore.h"
#include "Positioner.h"
#include "SubModel.h"

//a model is a collection of sub models and a positioner used across them

class CG3DModel : public CG3DResourceList<CG3DSubModel>
{
public:
	CG3DModel(CG3DEffect* effect) : m_Pos(new CG3DPositioner(effect)) { }
	
	virtual void ReleaseInternal() {}

	//set paremeters on one sub material or on all
	void SetParameter(const char* name, const float* values, int subModel = -1);
	void SetParameter(const char* name, const int* values, int subModel = -1);
	void SetTexture(const char* name, CG3DTexture* texture, int subModel = -1);

	void SetParameter(const char* name, CG3DVector2& value, int subModel = -1) { SetParameter(name, &value.x, subModel); }
	void SetParameter(const char* name, CG3DVector3& value, int subModel = -1) { SetParameter(name, &value.x, subModel); }
	void SetParameter(const char* name, CG3DVector4& value, int subModel = -1) { SetParameter(name, &value.x, subModel); }
	void SetParameter(const char* name, CG3DQuaternion& value, int subModel = -1) { SetParameter(name, &value.x, subModel); }
	void SetParameter(const char* name, CG3DMatrix& value, int subModel = -1) { SetParameter(name, &value.m11, subModel); }

	//set the positioners value
	void SetWorld(CG3DMatrix value) { m_World = value; m_Pos->Set(m_World); }
protected:
	virtual void AddInternal(CG3DSubModel* item) {}
	virtual void RemoveInternal(CG3DSubModel* item) {}
	virtual void UploadInternal() {}
	virtual void ReloadInternal() {}
	virtual void UnloadInternal() {}
	virtual void ApplyInternal() {}
public:
	CG3DMatrix m_World;
	CG3DPtr<CG3DPositioner> m_Pos;
};

#endif