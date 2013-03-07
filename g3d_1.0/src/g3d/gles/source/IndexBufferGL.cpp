#include "GraphicsGL.h"

void CG3DIndexBufferGL::Setup(Type type)
{
	switch (type & MASK)
	{
		case POINTS: m_Mode = GL_POINTS; break;
		case LINES: m_Mode = GL_LINES; break;
		case LINESTRIP: m_Mode = GL_LINE_STRIP; break;
		case TRIANGLES: m_Mode = GL_TRIANGLES; break;
		case TRIANGLESTRIP: m_Mode = GL_TRIANGLE_STRIP; break;
		case LINELOOP: m_Mode = GL_LINE_LOOP; break;
		case TRIANGLEFAN: m_Mode = GL_TRIANGLE_FAN; break;
	}
	switch (type & SIZE_MASK)
	{
		case SIZE_8: m_Type = GL_UNSIGNED_BYTE; m_Stride = 1; break;
		case SIZE_16: m_Type = GL_UNSIGNED_SHORT; m_Stride = 2; break;
		case SIZE_32: m_Type = GL_UNSIGNED_INT; m_Stride = 4; break;
	}
}

void CG3DIndexBufferGL::UploadInternal()
{
	glGenBuffers(1, &m_Buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	
	if (GetFlag(TRANSIENT))
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, m_Data, GL_STREAM_DRAW);
	else if (GetFlag(DYNAMIC))
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, m_Data, GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, m_Data, GL_STATIC_DRAW);
}

void CG3DIndexBufferGL::ReloadInternal()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_Size, m_Data);
}

void CG3DIndexBufferGL::UnloadInternal()
{
    glDeleteBuffers(1, &m_Buffer);
}

void CG3DIndexBufferGL::ApplyInternal()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffer);

	m_Graphics->ApplyEffect();

    glDrawElements(m_Mode, m_Size / m_Stride, m_Type, 0);
}