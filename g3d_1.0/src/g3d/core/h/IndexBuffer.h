#ifndef G3D_CORE_INDEX_BUFFER_H
#define G3D_CORE_INDEX_BUFFER_H

// This is a buffer that stores index data

#include "BufferCommon.h"
#include <stdlib.h>

#define INDEX_TYPE(type, size) (CG3DIndexBuffer::Type)((int)(CG3DIndexBuffer::##type) | (int)(CG3DIndexBuffer::##size))

class CG3DIndexBuffer : public CG3DBufferCommon {
public:
	enum Type {
		POINTS = 0,
		LINES = 1,
		LINESTRIP = 2,
		TRIANGLES = 3,
		TRIANGLESTRIP = 4,
		LINELOOP = 5,
		TRIANGLEFAN = 6,
		MASK = 7,

		SIZE_8 = 8,
		SIZE_16 = 16,
		SIZE_32 = 24,
		SIZE_MASK = 24,
	};
public:
	// set the primitive type and element size (use the INDEX_TYPE macro like this Setup(INDEX_TYPE(TRIANGLES, SIZE_16)); )
	// this sets the stride of the buffer
	virtual void Setup(Type type) = 0;
};

#endif