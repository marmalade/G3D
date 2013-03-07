#include "GraphicsGL.h"

void CG3DShaderGL::AddDefine(const char* name, const char* value)
{
	RemoveDefine(name);

	std::string str = name;
	str += " ";
	str += value;
	m_Defines.push_back(str);
}

void CG3DShaderGL::RemoveDefine(const char* name)
{
	for (int i=0; i<(int)m_Defines.size(); i++)
		if (m_Defines[i].substr(0, strlen(name)) == name)
		{
			m_Defines.erase(m_Defines.begin() + i);
			break;
		}
}

void CG3DShaderGL::ClearDefines()
{
	m_Defines.clear();
}

bool CG3DShaderGL::SetSource(Type type, const char* source)
{
	m_Type = (type == PIXEL) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
	m_Source = source;
	return true;
}

void* CG3DShaderGL::MakeBinaryBuffer(Type type, int length, unsigned int format)
{
	m_Type = (type == PIXEL) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;
	m_Binary = new char[length];
	m_BinaryLen = length;
	m_BinaryFormat = format;
	return m_Binary;
}

bool CG3DShaderGL::ValidFormat(unsigned int format)
{
	int num;
	int formats[32];

	glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &num);
	glGetIntegerv(GL_SHADER_BINARY_FORMATS, formats);

	for (int i=0; i<num; i++)
		if((unsigned int)formats[i] == format)
			return true;
	return false;
}

std::string CG3DShaderGL::MakeString()
{
	std::string str;
	int offset = m_Source.find("//insert defines here");
	if (offset != -1)
		str += m_Source.substr(0, offset);

	for (int i = 0; i < (int)m_Defines.size(); i++)
		str += "#define " + m_Defines[i] + "\n";
	
	if (offset != -1)
		str += m_Source.substr(offset);
	else
		str += m_Source;
	return str;
}

void CG3DShaderGL::UploadInternal()
{
	m_Shader = glCreateShader(m_Type);

	if (m_Binary != NULL)
	{
		glShaderBinary(1, &m_Shader, m_BinaryFormat, m_Binary, m_BinaryLen);
	}
	else
	{
		std::string result = MakeString();

		const char* str = result.c_str();
		const int strlen = result.size();
		glShaderSource(m_Shader, 1, &str, &strlen);
	}

	glCompileShader(m_Shader);
	
    GLint compileSuccess;
    glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &compileSuccess);
    GLchar messages[256];
    if (compileSuccess == GL_FALSE) {
        glGetShaderInfoLog(m_Shader, 256, NULL, &messages[0]);
		G3D_ASSERT_EXPR(compileSuccess, ("Shader error: %s", messages));
	}
}

void CG3DShaderGL::UnloadInternal()
{
	glDeleteShader(m_Shader);
}

void CG3DShaderGL::ReleaseInternal()
{
	m_Source.clear();
}