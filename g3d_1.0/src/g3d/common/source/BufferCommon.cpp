#include "BufferCommon.h"

void* CG3DBufferCommon::CreateBuffer(unsigned int num, unsigned int size)
{
	ReleaseInternal();

	if (size == USE_CURRENT_STRIDE)
	{
		if (m_Stride == 0)
			size = 1;
		else
			size = m_Stride;
	}

	m_Data = new unsigned char[num * size];
	m_Size = num * size;
	m_Stride = size;
	m_Offset = 0;

	return m_Data;
}

void CG3DBufferCommon::ReleaseInternal()
{
	if (m_Data != NULL)
		delete[] m_Data;
	m_Data = NULL;
}