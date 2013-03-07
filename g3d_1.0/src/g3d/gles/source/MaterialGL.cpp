#include "GraphicsGL.h"

void CG3DParamValueGL::Prepare(CG3DGraphicsGL* graphics)
{
	graphics->SetParameter(this);
}

void CG3DParamFValueGL::Set(const float* value)
{
	for (int i = 0; i < m_Size; i++)
		m_Data[i] = value[i];
}

void CG3DParamFValueGL::Apply(int slot, int& numTextures)
{
	switch (m_Type & CG3DEffectGL::COMPONENT_MASK) {
		case CG3DEffectGL::COMPONENT_1: 
			glUniform1fv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_2:
			glUniform2fv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_3:
			glUniform3fv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_4:
			glUniform4fv(slot, 1, m_Data);
			break;
	}
}

void CG3DParamMValueGL::Apply(int slot, int& numTextures)
{
	bool transpose = (m_Type & CG3DEffectGL::TYPE_MASK) == CG3DEffectGL::TYPE_TRANSPOSE_MATRIX;
	switch (m_Type & CG3DEffectGL::COMPONENT_MASK) {
		case CG3DEffectGL::COMPONENT_2x2:
			glUniformMatrix2fv(slot, 1, transpose, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_3x3:
			glUniformMatrix3fv(slot, 1, transpose, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_4x4:
			glUniformMatrix4fv(slot, 1, transpose, m_Data);
			break;
	}
}

void CG3DParamIValueGL::Set(const int* value)
{
	for (int i = 0; i < m_Size; i++)
		m_Data[i] = value[i];
}

void CG3DParamIValueGL::Apply(int slot, int& numTextures)
{
	switch (m_Type & CG3DEffectGL::COMPONENT_MASK) {
		case CG3DEffectGL::COMPONENT_1: 
			glUniform1iv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_2:
			glUniform2iv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_3:
			glUniform3iv(slot, 1, m_Data);
			break;
		case CG3DEffectGL::COMPONENT_4:
			glUniform4iv(slot, 1, m_Data);
			break;
	}
}

void CG3DParamTValueGL::Prepare(CG3DGraphicsGL* graphics)
{
	graphics->SetParameter(this);
}

void CG3DParamTValueGL::Set(const CG3DTexture* value)
{
	m_Texture = (CG3DTextureGL*)value;
}

void CG3DParamTValueGL::Apply(int slot, int& numTextures)
{
	glActiveTexture(GL_TEXTURE0 + numTextures);
	m_Texture->Apply();
	glUniform1i(slot, numTextures);
	numTextures++;
}

CG3DMaterialGL::CG3DMaterialGL(CG3DEffectGL* effect, bool fullMaterial) : m_Effect(effect), m_FullMaterial(fullMaterial)
{
	Add(effect);
}

void CG3DMaterialGL::AddSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set)
{
	set->AddRef();
	RemoveSet(type);
	m_Sets[type] = set;
}
void CG3DMaterialGL::RemoveSet(CG3DGlobalSet::SetType type)
{
	SetMap::iterator it = m_Sets.find(type);
	if (it == m_Sets.end())
		return;

	m_Sets[type]->Release();
	m_Sets.erase(it);
}

void CG3DMaterialGL::Apply()
{
	if (m_FullMaterial)
		m_Effect->Apply();

	CG3DResource::Apply();

	for (SetMap::iterator it=m_Sets.begin(); it!=m_Sets.end(); ++it)
		m_Effect->m_Graphics->PushGlobalSet(it->first, it->second);
}
void CG3DMaterialGL::DoneApply()
{
	for (SetMap::iterator it=m_Sets.begin(); it!=m_Sets.end(); ++it)
		m_Effect->m_Graphics->PopGlobalSet(it->first);
}

void CG3DMaterialGL::ApplyInternal()
{
	for (int i=0; i<(int)m_Params.size(); i++)
		m_Params[i]->Prepare(m_Effect->m_Graphics);
}

void CG3DMaterialGL::UnloadInternal()
{
	for (int i=0; i<(int)m_Params.size(); i++)
		delete m_Params[i];
	m_Params.clear();
}

CG3DParameterGL* CG3DMaterialGL::GetParameter(const char* name)
{
	for (int i=0; i<(int)m_Params.size(); i++)
		if (m_Params[i]->m_Name == name)
			return m_Params[i];

	CG3DEffectGL::Parameter* def = m_Effect->FindParameter(name);
	if (def == NULL) return NULL;

	CG3DParameterGL* result = NULL;
	switch (def->m_Type & CG3DEffectGL::TYPE_MASK) {
		case CG3DEffectGL::TYPE_FLOAT:
			result = new CG3DParamFValueGL(name, def->m_Type, def->m_Size);
			m_Params.push_back(result);
			break;
		case CG3DEffectGL::TYPE_INT:
			result = new CG3DParamIValueGL(name, def->m_Type, def->m_Size);
			m_Params.push_back(result);
			break;
		case CG3DEffectGL::TYPE_MATRIX:
		case CG3DEffectGL::TYPE_TRANSPOSE_MATRIX:
			result = new CG3DParamMValueGL(name, def->m_Type, def->m_Size);
			m_Params.push_back(result);
			break;
		case CG3DEffectGL::TYPE_TEXTURE:
			result = new CG3DParamTValueGL(name);
			m_Params.push_back(result);
			break;
	}
	return result;
}

void CG3DMaterialGL::SetParameter(const char* name, const float* values)
{
	CG3DParameterGL* param = GetParameter(name);
	G3D_ASSERT_RET(param != NULL, ("Param %s not found", name));

	param->Set(values);
}

void CG3DMaterialGL::SetParameter(const char* name, const int* values)
{
	CG3DParameterGL* param = GetParameter(name);
	G3D_ASSERT_RET(param != NULL, ("Param %s not found", name));

	param->Set(values);
}

void CG3DMaterialGL::SetTexture(const char* name, CG3DTexture* texture)
{
	CG3DParameterGL* param = GetParameter(name);
	G3D_ASSERT_RET(param != NULL, ("Param %s not found", name));
	
	Add(texture);
	param->Set(texture);
}