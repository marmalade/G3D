#ifndef G3D_CORE_SHADER_H
#define G3D_CORE_SHADER_H

// This is a vertex or pixel shader

#include "Resource.h"
#include <stdlib.h>

class CG3DShader : public CG3DResource {
public:
	enum Type {
		VERTEX,
		PIXEL,
		MAX,
	};
public:
	// set the source text of the shader, glsl style
	virtual bool SetSource(Type type, const char* source) { return false; }
	// set the source text of the shader, hlsl style
	virtual bool SetSource(const char* target, const char* entry, const char* source, const char* filename) { return false; }
	// make a buffer to copy the binary data into, returns NULL if format not supported (format is used for glsl)
	virtual void* MakeBinaryBuffer(Type type, int length, unsigned int format = 0) { return NULL; }
	// check binary format is available (format 0 is used for hlsl)
	virtual bool ValidFormat(unsigned int format) { return false; }
	// add a define
	virtual void AddDefine(const char* name, const char* value) = 0;
	// remove a define
	virtual void RemoveDefine(const char* name) = 0;
	// clear all defines
	virtual void ClearDefines() = 0;
};

#endif