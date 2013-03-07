#ifndef G3D_CORE_VERTEX_BUFFER_H
#define G3D_CORE_VERTEX_BUFFER_H

// a buffer containing vertex data and a specification of the vertex data

#include "BufferCommon.h"
#include <stdlib.h>
#include "Material.h"

#define ELEMENT_TYPE(component, size, type) (CG3DVertexBuffer::ElementType)((int)(CG3DVertexBuffer::##component) | (int)(CG3DVertexBuffer::##size) | (int)(CG3DVertexBuffer::##type))
#define ELEMENT_TYPE_NORM(component, size, type) (CG3DVertexBuffer::ElementType)((int)ELEMENT_TYPE(component, size, type) | (int)(CG3DVertexBuffer::NORMALIZE))
#define ELEMENT_TYPE_NORM_SRGB(component, size, type) (CG3DVertexBuffer::ElementType)((int)ELEMENT_TYPE(component, size, type) | (int)(CG3DVertexBuffer::NORMALIZE_SRGB))

#define ELEMENT_OFFSET_PACK -1

class CG3DVertexBuffer : public CG3DBufferCommon {
public:
	enum ElementType {
		COMPONENT_1 = 0,
		COMPONENT_2 = 1,
		COMPONENT_3 = 2,
		COMPONENT_4 = 3,
		COMPONENT_MASK = 3,
		
		SIZE_8 = 4,
		SIZE_16 = 8,
		SIZE_32 = 12,
		SIZE_MASK = 12,

		TYPE_FLOAT = 16,
		TYPE_UNSIGNED = 32,
		TYPE_SIGNED = 48,
		TYPE_MASK = 48,

		NORMALIZE = 64,
		NORMALIZE_SRGB = 192,
	};
public:
	// add a specification of vertex data (if offset is ELEMENT_OFFSET_PACK automatically assign offset based on packing)
	// this sets the stride of the buffer
	virtual void AddElement(const char* name,ElementType type, int offset = ELEMENT_OFFSET_PACK) = 0;
	// use an effect to convert element names to slot numbers, this has to be done before drawing
	void PatchUp(CG3DMaterial* mat, CG3DVertexBuffer** auxBufferList = NULL) { PatchUp(mat->GetEffect(), auxBufferList); }
	virtual void PatchUp(CG3DEffect* effect, CG3DVertexBuffer** auxBufferList = NULL) = 0;
};

#endif