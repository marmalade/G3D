#ifndef G3D_HELPER_SUB_MODEL_H
#define G3D_HELPER_SUB_MODEL_H

#include "G3DCore.h"
#include "Positioner.h"

//a sub model has one material, one reference to a positioner and a set of vertex and index buffers

class CG3DSubModelData
{
};

class CG3DSubModel : public CG3DResourceList<CG3DResource>
{
public:
	CG3DSubModel(CG3DPositioner* pos) : m_Pos(pos), m_Material(NULL), m_Data(NULL) { Add(pos); }
	
	//add or remove material, positioner, vertex buffers or index buffers
	virtual void Add(CG3DResource* item);
	virtual void Remove(CG3DResource* item);
	virtual void ReleaseInternal() {}
	
	virtual void Apply();	//apply the positioner, material, and vertex and index buffers
	void ApplyMesh();		//apply the vertex and index buffers

	//set parameters on the material
	void SetParameter(const char* name, const float* values) { if (m_Material != NULL) m_Material->SetParameter(name, values); }
	void SetParameter(const char* name, const int* values) { if (m_Material != NULL) m_Material->SetParameter(name, values); }
	void SetTexture(const char* name, CG3DTexture* texture)  { if (m_Material != NULL) m_Material->SetTexture(name, texture); }

	void SetParameter(const char* name, CG3DVector2& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DVector3& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DVector4& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DQuaternion& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DMatrix& value) { SetParameter(name, &value.m11); }
protected:
	virtual void AddInternal(CG3DResource* item);
	virtual void RemoveInternal(CG3DResource* item) {}
	virtual void UploadInternal();
	virtual void ReloadInternal() { UploadInternal(); }
	virtual void UnloadInternal() {}
	virtual void ApplyInternal() {}
public:
	CG3DPositioner* m_Pos;
	CG3DMaterial* m_Material;
	CG3DSubModelData* m_Data;	//arbitrary data used by some scenes
};

#endif