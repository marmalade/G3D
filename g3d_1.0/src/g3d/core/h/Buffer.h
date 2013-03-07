#ifndef G3D_CORE_BUFFER_H
#define G3D_CORE_BUFFER_H

// Buffer is a type of resource that gets a block of data and uploads it to the graphics system
// The data is deleted after upload if the DYNAMIC flag is false otherwise on release.
// Firstly you call CreateBuffer with a size, this creates a buffer internally, then
// There are 3 ways to fill the buffer:
// 1) CreateBuffer returns a pointer to the buffer, copy into this.
// 2) make a series of Add calls, which copies data into the created buffer and increments
//    the add offset
// 3) make a series of Set calls, which copies data into the created buffer at an offset
// data size can be specified in 1 dimension (num) or in 2 dimensions (num * size)
// most buffers in the graphics system are specified in 2 dimensions (textures, vertex and index buffers)

#include "Resource.h"
#include <stdlib.h>

#define USE_CURRENT_STRIDE -1

class CG3DBuffer : public CG3DResource {
protected:
	enum BufferInternalFlag {
		OWN_BUFFER = 2, // delete the buffer when finished with it, unused
	};
public:
	CG3DBuffer() : m_Data(NULL), m_Size(0), m_Stride(0), m_Offset(0) {}

	// create a buffer of a particular size
	// if size is USE_CURRENT_STRIDE then use the current stride (set by CG3DVertexBuffer::AddElement's or CG3DIndexBuffer::Setup)
	virtual void* CreateBuffer(unsigned int num, unsigned int size = USE_CURRENT_STRIDE) = 0;

	// copy data into the buffer and increment the insert point
	template<class T> void Add(T data) {
		memcpy(m_Data + m_Offset, &data, sizeof(T));
		m_Offset += sizeof(T);
	}
	// copy data into the buffer at a specified point
	template<class T> void Set(int offset, T data) {
		memcpy(m_Data + offset, &data, sizeof(T));
	}
protected:
	unsigned char* m_Data;
	unsigned int m_Size;
	unsigned int m_Stride;
	unsigned int m_Offset;
};

#endif