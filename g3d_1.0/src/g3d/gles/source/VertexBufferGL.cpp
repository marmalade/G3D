#include "GraphicsGL.h"

void CG3DVertexBufferGL::AddElement(const char* name,ElementType type, int offset)
{
	int stride = 0;
	Element elem;

	elem.m_Name=name;
	elem.m_Offset=-1;

	switch (type & COMPONENT_MASK)
	{
		case COMPONENT_1: elem.m_Size = 1; break;
		case COMPONENT_2: elem.m_Size = 2; break;
		case COMPONENT_3: elem.m_Size = 3; break;
		case COMPONENT_4: elem.m_Size = 4; break;
	}
	switch (type & TYPE_MASK)
	{
		case TYPE_FLOAT: elem.m_Type = GL_FLOAT; stride = 4; break;
		case TYPE_UNSIGNED:
			switch (type & SIZE_MASK)
			{
				case SIZE_8: elem.m_Type = GL_UNSIGNED_BYTE; stride = 1; break;
				case SIZE_16: elem.m_Type = GL_UNSIGNED_SHORT; stride = 2; break;
				case SIZE_32: elem.m_Type = GL_FIXED; stride = 4; break;
			}
			break;
		case TYPE_SIGNED:
			switch (type & SIZE_MASK)
			{
				case SIZE_8: elem.m_Type = GL_BYTE; stride = 1; break;
				case SIZE_16: elem.m_Type = GL_SHORT; stride = 2; break;
				case SIZE_32: elem.m_Type = GL_FIXED; stride = 4; break;
			}
			break;
	}
	elem.m_Normalized = (type & NORMALIZE) == NORMALIZE;
	if (offset == ELEMENT_OFFSET_PACK)
	{
		elem.m_Pointer = (GLvoid*)m_Stride;
	}
	else
	{
		elem.m_Pointer = (GLvoid*)offset;
		m_Stride = offset;
	}
	m_Stride += elem.m_Size * stride;
	m_Elements.push_back(elem);
}

void CG3DVertexBufferGL::PatchUp(CG3DEffect* effect, CG3DVertexBuffer** auxBufferList)
{
	for (int i=0; i<(int)m_Elements.size(); i++)
		m_Elements[i].m_Offset = ((CG3DEffectGL*)effect)->GetVertexSlot(m_Elements[i].m_Name.c_str());
	if (auxBufferList != NULL)
		for (int i=0; auxBufferList[i] != NULL; i++)
			auxBufferList[i]->PatchUp(effect);
}

void CG3DVertexBufferGL::UploadInternal()
{
	glGenBuffers(1, &m_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	
	if (GetFlag(TRANSIENT))
		glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data, GL_STREAM_DRAW);
	else if (GetFlag(DYNAMIC))
		glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data, GL_DYNAMIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data, GL_STATIC_DRAW);
}

void CG3DVertexBufferGL::ReloadInternal()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_Size, m_Data);
}

void CG3DVertexBufferGL::UnloadInternal()
{
    glDeleteBuffers(1, &m_Buffer);
}

void CG3DVertexBufferGL::ApplyInternal()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);

	unsigned int max = 0;
	for (int i = 0; i < (int)m_Elements.size(); i++)
	{
		if (m_Elements[i].m_Offset == -1)
			continue;
		glVertexAttribPointer(m_Elements[i].m_Offset, m_Elements[i].m_Size, m_Elements[i].m_Type, m_Elements[i].m_Normalized, m_Stride, m_Elements[i].m_Pointer);
		if (m_Elements[i].m_Offset+1 > max)
			max = m_Elements[i].m_Offset+1;
	}

	if (max > m_Graphics->m_NumAttrib)
	{
		for (unsigned int i = m_Graphics->m_NumAttrib; i < max; i++)
			glEnableVertexAttribArray(i);
	}
	else if (max < m_Graphics->m_NumAttrib)
	{
		for (unsigned int i = max; i < m_Graphics->m_NumAttrib; i++)
			glDisableVertexAttribArray(i);
	}
	m_Graphics->m_NumAttrib = max;
}