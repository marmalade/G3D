#ifndef G3D_CORE_EFFECT_H
#define G3D_CORE_EFFECT_H

// Effect is a set of shaders and a list of parameters those shaders expose
// Effects are abstract, call MakeMaterial to make an implementation of
// an effect with parameters set to specific values

#include "ResourceList.h"
#include "Shader.h"
#include <stdlib.h>

class CG3DMaterial;

#define PARAMETER_TYPE(component, type) (CG3DEffect::ParameterType)((int)(CG3DEffect::component) | (int)(CG3DEffect::type))

class CG3DEffect : public CG3DResourceList<CG3DShader> {
public:
	enum ParameterType {
		COMPONENT_1 = 0,
		COMPONENT_2 = 1,
		COMPONENT_3 = 2,
		COMPONENT_4 = 3,
		COMPONENT_2x2 = 1,
		COMPONENT_3x3 = 2,
		COMPONENT_4x4 = 3,
		COMPONENT_MASK = 3,

		TYPE_FLOAT = 0,
		TYPE_INT = 4,
		TYPE_MATRIX = 8,
		TYPE_TRANSPOSE_MATRIX = 12,
		TYPE_TEXTURE = 16,
		TYPE_MASK = 28,
	};
public:
	// make a shader tied to this effect
	virtual CG3DShader* MakeShader() = 0;
	// make a material that implements this effect
	virtual CG3DMaterial* MakeMaterial() = 0;
	// make a sub material that implements part of this effect, but is shared
	// if the platform implements constant buffers, they have to belong to one constant buffer
	virtual CG3DMaterial* MakeSubMaterial(const char* bufferName) = 0;
	// test if the system supports program binaries of type format
	virtual bool ValidProgramFormat(unsigned int format) = 0;
	// if the system supports it set a program binary
	virtual bool SetProgramBinary(void* data, int size, unsigned int format) = 0;
	
	// add an input parameter to the shaders, use the PARAMETER_TYPE macro to specify the type like this:
	//   AddParameter("Name", PARAMETER_TYPE(COMPONENT_4, TYPE_FLOAT));
	virtual void AddParameter(const char* name, ParameterType type, const char* auxName = NULL) = 0;
	// add a texture input parameter, for hlsl specify the sampler in auxName
	virtual void AddTexture(const char* name, const char* auxName = NULL) { AddParameter(name, PARAMETER_TYPE(COMPONENT_1, TYPE_TEXTURE), auxName); }
protected:
	virtual void Add(CG3DShader* item) { CG3DResourceList<CG3DShader>::Add(item); }
	virtual void Remove(CG3DShader* item) { CG3DResourceList<CG3DShader>::Remove(item); }
};

#endif