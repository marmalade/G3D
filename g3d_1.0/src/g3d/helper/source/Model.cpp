#define _CRT_SECURE_NO_WARNINGS

#include "Model.h"
#include <stdio.h>
#include <string.h>

void CG3DModel::SetParameter(const char* name, const float* values, int subModel)
{
	if (subModel == -1)
		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->SetParameter(name, values);
	else if (subModel <= (int)m_List.size())
		m_List[subModel]->SetParameter(name, values);
}
void CG3DModel::SetParameter(const char* name, const int* values, int subModel)
{
	if (subModel == -1)
		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->SetParameter(name, values);
	else if (subModel <= (int)m_List.size())
		m_List[subModel]->SetParameter(name, values);
}
void CG3DModel::SetTexture(const char* name, CG3DTexture* texture, int subModel)
{
	if (subModel == -1)
		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->SetTexture(name, texture);
	else if (subModel <= (int)m_List.size())
		m_List[subModel]->SetTexture(name, texture);
}
