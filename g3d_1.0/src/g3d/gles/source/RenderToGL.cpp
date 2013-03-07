#include "GraphicsGL.h"

void CG3DRenderToGL::Release(BufferType type)
{
	Buffer& buffer = m_Buffers[type];
	if (buffer.m_Texture != NULL)
		Remove(buffer.m_Texture);
	buffer.m_Texture = NULL;

	buffer.m_Format = FORMAT_NONE;
}

void CG3DRenderToGL::SetBuffer(BufferType type, BufferFormat format, int width, int height)
{
	Release(type);

	Buffer& buffer = m_Buffers[type];

	switch(format)
	{
		case FORMAT_NONE: buffer.m_Format = GL_NONE; break;
		case FORMAT_DEPTH_16: buffer.m_Format = GL_DEPTH_COMPONENT16; break;
		case FORMAT_RGBA_4444: buffer.m_Format = GL_RGBA4; break;
		case FORMAT_RGBA_5551: buffer.m_Format = GL_RGB5_A1; break;
		case FORMAT_RGB_565: buffer.m_Format = GL_RGB565; break;
		case FORMAT_STENCIL_8: buffer.m_Format = GL_STENCIL_INDEX8; break;
	}
	buffer.m_Width = width;
	buffer.m_Height = height;
}

void CG3DRenderToGL::SetTexture(BufferType type, CG3DTexture* texture, CG3DTexture::Target target, int mipmap)
{
	Release(type);

	Buffer& buffer = m_Buffers[type];

	buffer.m_Texture = (CG3DTextureGL*)texture;
	Add(texture);
	switch (target)
	{
		case CG3DTextureGL::TARGET_POS_X: buffer.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
		case CG3DTextureGL::TARGET_POS_Y: buffer.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
		case CG3DTextureGL::TARGET_POS_Z: buffer.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;
		case CG3DTextureGL::TARGET_NEG_X: buffer.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
		case CG3DTextureGL::TARGET_NEG_Y: buffer.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
		case CG3DTextureGL::TARGET_NEG_Z: buffer.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
		default: buffer.m_Target = GL_TEXTURE_2D; break;
	}
	buffer.m_Mipmap = mipmap;
}

void CG3DRenderToGL::AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set)
{
	set->AddRef();
	RemoveSet(type);
	m_Sets[type] = set;
}
void CG3DRenderToGL::RemoveSet(CG3DGlobalSet::SetType type)
{
	SetMap::iterator it = m_Sets.find(type);
	if (it == m_Sets.end())
		return;

	m_Sets[type]->Release();
	m_Sets.erase(it);
}

void CG3DRenderToGL::UploadInternal()
{
	GLenum attach[] = {
		GL_COLOR_ATTACHMENT0,
		GL_DEPTH_ATTACHMENT,
		GL_STENCIL_ATTACHMENT
	};
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&m_PrevFrameBuffer);

	glGenFramebuffers(1, &m_FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

	for (int i=0; i<3; i++)
	{
		if (m_Buffers[i].m_Texture != NULL)
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, attach[i], m_Buffers[i].m_Target,
				m_Buffers[i].m_Texture->m_Texture, m_Buffers[i].m_Mipmap);
		}
		else if (m_Buffers[i].m_Format != GL_NONE)
		{
			glGenRenderbuffers(1, &m_Buffers[i].m_Buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_Buffers[i].m_Buffer);
			glRenderbufferStorage(GL_RENDERBUFFER, m_Buffers[i].m_Format,
				m_Buffers[i].m_Width, m_Buffers[i].m_Height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attach[i], GL_RENDERBUFFER, m_Buffers[i].m_Buffer);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, m_PrevFrameBuffer);
}

void CG3DRenderToGL::UnloadInternal()
{
	for (int i=0; i<3; i++)
		if (m_Buffers[i].m_Format != GL_NONE)
			glDeleteRenderbuffers(1, &m_Buffers[i].m_Buffer);

	glDeleteFramebuffers(1, &m_FrameBuffer);
}

void CG3DRenderToGL::ApplyInternal()
{
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&m_PrevFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

	for (SetMap::iterator it=m_Sets.begin(); it!=m_Sets.end(); ++it)
		m_Graphics->PushGlobalSet(it->first, it->second);
}

void CG3DRenderToGL::DoneApply()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_PrevFrameBuffer);

	for (SetMap::iterator it=m_Sets.begin(); it!=m_Sets.end(); ++it)
		m_Graphics->PopGlobalSet(it->first);
}