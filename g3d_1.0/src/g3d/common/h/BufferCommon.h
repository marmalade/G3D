#ifndef G3D_CORE_BUFFER_COMMON_H
#define G3D_CORE_BUFFER_COMMON_H

#include "Buffer.h"
#include <stdlib.h>

class CG3DBufferCommon : public CG3DBuffer {
public:
	virtual void* CreateBuffer(unsigned int num, unsigned int size = USE_CURRENT_STRIDE);

protected:
	virtual void ReleaseInternal();
};

#endif