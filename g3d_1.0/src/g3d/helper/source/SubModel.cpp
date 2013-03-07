#define _CRT_SECURE_NO_WARNINGS

#include "Model.h"
#include <stdio.h>
#include <string.h>

void CG3DSubModel::Apply()
{
	m_Pos->Apply();

	m_Material->Apply();

	ApplyMesh();
	
	m_Material->DoneApply();
}

void CG3DSubModel::ApplyMesh()
{
	for (int i=0; i<(int)m_List.size(); i++)
		if (m_Material != m_List[i] && m_Pos != m_List[i])
			m_List[i]->Apply();
}

void CG3DSubModel::Add(CG3DResource* item)
{
	CG3DMaterial* mat = dynamic_cast<CG3DMaterial*>(item);
	if (mat != NULL)
		m_Material = mat;

	CG3DResourceList<CG3DResource>::Add(item);
}

void CG3DSubModel::AddInternal(CG3DResource* item)
{
	if (m_Material == item)
		return;

	CG3DVertexBuffer* vb = dynamic_cast<CG3DVertexBuffer*>(item);
	if (vb != NULL && m_Material != NULL)
		vb->PatchUp(m_Material);
}
	
void CG3DSubModel::Remove(CG3DResource* item)
{
	if (m_Material == item)
		m_Material = NULL;
}

void CG3DSubModel::UploadInternal()
{
	for (int i=0; i<(int)m_List.size(); i++)
	{
		CG3DVertexBuffer* vb = dynamic_cast<CG3DVertexBuffer*>(m_List[i]);
		if (vb != NULL && m_Material != NULL)
			vb->PatchUp(m_Material);
	}
}