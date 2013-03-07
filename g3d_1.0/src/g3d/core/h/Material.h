#ifndef G3D_CORE_MATERIAL_H
#define G3D_CORE_MATERIAL_H

// material to be applied to a sub-model (of a sub material)
// materials point to an effect which has the shaders,
// the material stores the parameter values and textures
// sub-materials encapsulate a subset of parameters (on hlsl this maps to a constant buffer)
//  that may be applied more generally (for instance a perspective matrix set once for the entire scene)

#include "ResourceList.h"
#include "Texture.h"
#include "GlobalSet.h"
#include <stdlib.h>

class CG3DEffect;

class CG3DMaterial : public CG3DResourceList<CG3DResource> {
public:
	// set the value of a parameter
	virtual void SetParameter(const char* name, const float* values) = 0;
	virtual void SetParameter(const char* name, const int* values) = 0;
	// set a parameter to be this texture
	virtual void SetTexture(const char* name, CG3DTexture* texture) = 0;
	
	// set the value of a parameter for common types
	void SetParameter(const char* name, CG3DVector2& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DVector3& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DVector4& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DQuaternion& value) { SetParameter(name, &value.x); }
	void SetParameter(const char* name, CG3DMatrix& value) { SetParameter(name, &value.m11); }

	// get the effect
	virtual CG3DEffect* GetEffect() = 0;

	// add a global state to be set during this material
	virtual void AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set) = 0;
	// remove a global state
	virtual void RemoveSet(CG3DGlobalSet::SetType type) = 0;

	// finished applying this material (removes any global state sets)
	virtual void DoneApply() = 0;
protected:
	virtual void Add(CG3DResource* item) { CG3DResourceList<CG3DResource>::Add(item); }
	virtual void Remove(CG3DResource* item) { CG3DResourceList<CG3DResource>::Remove(item); }
};

#endif