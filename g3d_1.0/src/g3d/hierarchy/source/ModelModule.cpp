#include "ModelModule.h"
#include "GameObject.h"

CG3DModelModule::CG3DModelModule(CG3DScene* scene, CG3DMesh* mesh, CG3DMaterial** mats) : m_Scene(scene)
{
	m_Pos = new CG3DPositioner(mats[0]->GetEffect());
	m_NumSubModels = mesh->NumSubModels();
	m_SubModels = new CG3DSubModel*[m_NumSubModels];
	mesh->MakeSubModels(m_Pos.get(), mats, m_SubModels);
}

void CG3DModelModule::SetLocal(CG3DMatrix local)
{
	CG3DDummyModule::SetLocal(local);
	m_Pos->Set(m_Parent->m_WorldMatrix);
}
void CG3DModelModule::EnableInternal()
{
	CG3DDummyModule::EnableInternal();
	m_Scene->Add(m_SubModels, m_NumSubModels);
}
void CG3DModelModule::DisableInternal()
{
	m_Scene->Remove(m_SubModels, m_NumSubModels);
	CG3DDummyModule::DisableInternal();
}