#include "GraphicsGL.h"

CG3DMaterial* CG3DEffectGL::MakeMaterial()
{
	return new CG3DMaterialGL(this, true);
}

CG3DMaterial* CG3DEffectGL::MakeSubMaterial(const char* bufferName)
{
	return new CG3DMaterialGL(this, false);
}

CG3DShader* CG3DEffectGL::MakeShader()
{
	CG3DShader* shader = new CG3DShaderGL();
	Add(shader);
	return shader;
}

void CG3DEffectGL::AddParameter(const char* name, ParameterType type, const char* auxName)
{
	Parameter param;

	strcpy(param.m_Name, name);
	param.m_Type = type;

	switch (type & TYPE_MASK) {
		case TYPE_FLOAT:
		case TYPE_INT:
			switch (type & COMPONENT_MASK) {
				case COMPONENT_1: param.m_Size = 1; break;
				case COMPONENT_2: param.m_Size = 2; break;
				case COMPONENT_3: param.m_Size = 3; break;
				case COMPONENT_4: param.m_Size = 4; break;
			}
			break;
		case TYPE_MATRIX:
		case TYPE_TRANSPOSE_MATRIX:
			switch (type & COMPONENT_MASK) {
				case COMPONENT_2x2: param.m_Size = 4; break;
				case COMPONENT_3x3: param.m_Size = 9; break;
				case COMPONENT_4x4: param.m_Size = 16; break;
			}
			break;
		case TYPE_TEXTURE:
			param.m_Size = 1;
			break;
	}
	
	int numInts = 0, numTextures = 0, numFloats = 0;
	switch (type & TYPE_MASK)
	{
		case TYPE_INT:
			param.m_Offset = numInts;
			numInts += param.m_Size;
			break;
		case TYPE_TEXTURE:
			param.m_Offset = numTextures;
			numTextures += param.m_Size;
			break;
		default:
			param.m_Offset = numFloats;
			numFloats += param.m_Size;
			break;
	}

	param.m_Index = m_Parameters.size();
	m_Parameters.push_back(param);
}

int CG3DEffectGL::GetVertexSlot(const char* name)
{
   return glGetAttribLocation(m_Program,name);
}

bool CG3DEffectGL::ValidProgramFormat(unsigned int format)
{
	int num;
	int formats[32];

	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES, &num);
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES, formats);

	for (int i=0; i<num; i++)
		if((unsigned int)formats[i] == format)
			return true;
	return false;
}

bool CG3DEffectGL::SetProgramBinary(void* data, int size, unsigned int format)
{
	if (m_Program == 0xffffffff)
		m_Program = glCreateProgram();

	PFNGLPROGRAMBINARYOESPROC fn = (PFNGLPROGRAMBINARYOESPROC)eglGetProcAddress("glProgramBinaryOES");
	if (fn == NULL) return false;

	fn(m_Program, format, data, size);

	m_DoLink = false;

	return true;
}

CG3DEffectGL::Parameter* CG3DEffectGL::FindParameter(const char* name)
{
	for (int i = 0; i < (int)m_Parameters.size(); i++)
		if (!strcmp(m_Parameters[i].m_Name, name))
			return &m_Parameters[i];
	return NULL;
}
	
void CG3DEffectGL::DoApply(std::map<std::string, CG3DParameterGL*>& params, bool dirty)
{
	int numTextures = 0;
	for (int i=0; i<(int)m_Parameters.size(); i++)
	{
		CG3DParameterGL* param = params[m_Parameters[i].m_Name];
		if (param == NULL) continue;

		if (dirty || param->m_Dirty)
			param->Apply(m_Parameters[i].m_Location, numTextures);
	}
}

void CG3DEffectGL::AddInternal(CG3DShader* item)
{
	if (m_Program == 0xffffffff)
		m_Program = glCreateProgram();
	glAttachShader(m_Program, ((CG3DShaderGL*)item)->m_Shader);
}

void CG3DEffectGL::RemoveInternal(CG3DShader* item)
{
	glDetachShader(m_Program, ((CG3DShaderGL*)item)->m_Shader);
}

void CG3DEffectGL::UploadInternal()
{
	if (m_DoLink)
	{
		glLinkProgram(m_Program);

		GLint linkSuccess;
		glGetProgramiv(m_Program, GL_LINK_STATUS, &linkSuccess);
		GLchar messages[256];
		if (linkSuccess == GL_FALSE)
		{
			glGetProgramInfoLog(m_Program, 256, NULL, &messages[0]);
			G3D_ASSERT_EXPR(linkSuccess, ("Shader error: %s", messages));
		}
	}

	for (int i = 0; i < (int)m_Parameters.size(); i++)
		m_Parameters[i].m_Location = glGetUniformLocation(m_Program, m_Parameters[i].m_Name);
}

void CG3DEffectGL::UnloadInternal()
{
	glDeleteProgram(m_Program);
}

void CG3DEffectGL::ApplyInternal()
{
    glUseProgram(m_Program);

	m_Graphics->SetEffect(this);
}