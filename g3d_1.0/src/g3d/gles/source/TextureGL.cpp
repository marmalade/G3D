#include "GraphicsGL.h"

int CG3DTextureGL::Image::SetFormat(ImageFormat format)
{
	int stride = 3;
	//TODO check for other formats, add compressed support
	
	switch (format.type)
	{
		case  ImageFormat::Unsigned:
		case  ImageFormat::Signed:
			break;
		default:
			return 0;
	}
	switch (format.layout)
	{
		case ImageFormat::A: m_Format = GL_ALPHA; stride = 1; break;
		case ImageFormat::RGB: m_Format = GL_RGB; break;
		case ImageFormat::RGBA: m_Format = GL_RGBA; stride = 4; break;
		case ImageFormat::L: m_Format = GL_LUMINANCE; stride = 1; break;
		case ImageFormat::LA: m_Format = GL_LUMINANCE_ALPHA; stride = 2; break;
		default: return 0;
	}
	switch (format.depth)
	{
		case ImageFormat::D8: m_Type = GL_UNSIGNED_BYTE; break;
		case ImageFormat::D5_6_5: m_Type = GL_UNSIGNED_SHORT_5_6_5; stride = 2; break;
		case ImageFormat::D5_5_5_1: m_Type = GL_UNSIGNED_SHORT_5_5_5_1; stride = 2; break;
		case ImageFormat::D4_4_4_4: m_Type = GL_UNSIGNED_SHORT_4_4_4_4; stride = 2; break;
		default: return 0;
	}
	//if compressed m_Format is compression format, m_Type is 0

	return stride;
}

bool CG3DTextureGL::ValidFormat(ImageFormat format)
{
	Image image;
	return image.SetFormat(format) > 0;
}

void CG3DTextureGL::Setup(bool cube, bool generateMipmaps, MipMap filter, Wrap s, Wrap t)
{
	m_Target = cube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D;
	m_Generate = generateMipmaps;
	
	m_Mag = (filter & MAG_NEAREST) ? GL_NEAREST : GL_LINEAR;
	switch ((int)filter & 51)
	{
		case MIN_NEAREST: m_Min = GL_NEAREST; break;
		case MIN_LINEAR: m_Min = GL_LINEAR; break;
		case MIN_NEAREST | MIP_NEAREST: m_Min = GL_NEAREST_MIPMAP_NEAREST; break;
		case MIN_LINEAR | MIP_NEAREST: m_Min = GL_LINEAR_MIPMAP_NEAREST; break;
		case MIN_LINEAR | MIP_LINEAR: m_Min = GL_LINEAR_MIPMAP_LINEAR; break;
		default: m_Min = GL_NEAREST_MIPMAP_LINEAR; break;
	}
	switch (s)
	{
		case CLAMP: m_S = GL_CLAMP_TO_EDGE; break;
		case MIRRORED: m_S = GL_MIRRORED_REPEAT; break;
		default: m_S = GL_REPEAT; break;
	}
	switch (t)
	{
		case CLAMP: m_T = GL_CLAMP_TO_EDGE; break;
		case MIRRORED: m_T = GL_MIRRORED_REPEAT; break;
		default: m_T = GL_REPEAT; break;
	}
}

void* CG3DTextureGL::AddImage(Target target,ImageFormat format,int width, int height, int dataSize, int mipmap)
{
	Image image;
	int stride = image.SetFormat(format);
	if (stride == 0) return NULL;

	switch (target)
	{
		case TARGET_POS_X: image.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_X; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		case TARGET_POS_Y: image.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		case TARGET_POS_Z: image.m_Target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		case TARGET_NEG_X: image.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		case TARGET_NEG_Y: image.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		case TARGET_NEG_Z: image.m_Target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_CUBE_MAP, ("This is not a cube texture")); break;
		default: image.m_Target = GL_TEXTURE_2D; G3D_ASSERT_RETN(m_Target == GL_TEXTURE_2D, ("This is a cube texture")); break;
	}

	if (dataSize == 0)
	{
		G3D_ASSERT_RETN(image.m_Type != 0, ("dataSize needs to be >0 for compressed images"));
		dataSize = width*height*stride;
	}
	image.m_Level = mipmap;
	image.m_Width = width;
	image.m_Height = height;
	image.m_Data = new char[dataSize];
	image.m_DataSize = dataSize;
	m_Images.push_back(image);

	return image.m_Data;
}

void CG3DTextureGL::UploadInternal()
{
	glGenTextures(1, &m_Texture);
	glBindTexture(m_Target, m_Texture);

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, m_Min);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, m_Mag);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, m_S);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, m_T);
		
	for (int i=0; i<(int)m_Images.size(); i++)
	{
		if (m_Images[i].m_Type != 0)
			glTexImage2D(m_Images[i].m_Target, m_Images[i].m_Level, m_Images[i].m_Format, m_Images[i].m_Width, m_Images[i].m_Height, 0,
				m_Images[i].m_Format, m_Images[i].m_Type, m_Images[i].m_Data);
		else
			glCompressedTexImage2D(m_Images[i].m_Target, m_Images[i].m_Level, m_Images[i].m_Format, m_Images[i].m_Width, m_Images[i].m_Height, 0,
				m_Images[i].m_DataSize, m_Images[i].m_Data);
	}
	if (m_Generate)
		glGenerateMipmap(m_Target);
}

void CG3DTextureGL::ReloadInternal()
{
	glBindTexture(m_Target, m_Texture);

    glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, m_Min);
    glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, m_Mag);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, m_S);
    glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, m_T);

	for (int i=0; i<(int)m_Images.size(); i++)
	{
		if (m_Images[i].m_Type != 0)
			glTexSubImage2D(m_Images[i].m_Target, m_Images[i].m_Level, 0, 0, m_Images[i].m_Width, m_Images[i].m_Height,
				m_Images[i].m_Format, m_Images[i].m_Type, m_Images[i].m_Data);
		else
			glCompressedTexSubImage2D(m_Images[i].m_Target, m_Images[i].m_Level, 0, 0, m_Images[i].m_Width, m_Images[i].m_Height,
				m_Images[i].m_Format, m_Images[i].m_DataSize, m_Images[i].m_Data);
	}
	if (m_Generate)
		glGenerateMipmap(m_Target);
}

void CG3DTextureGL::UnloadInternal()
{
	glDeleteTextures(1, &m_Texture);
}

void CG3DTextureGL::ApplyInternal()
{
	glBindTexture(m_Target, m_Texture);
}

void CG3DTextureGL::ReleaseInternal()
{
	for (int i=0; i<(int)m_Images.size(); i++)
		if(m_Images[i].m_Data != NULL)
			delete[] m_Images[i].m_Data;
	m_Images.clear();
}