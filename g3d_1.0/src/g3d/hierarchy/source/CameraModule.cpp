#include "CameraModule.h"

void CG3DCameraModule::SetLocal(CG3DMatrix local)
{
	CG3DDummyModule::SetLocal(local);
	m_Camera->SetView(m_Parent->m_WorldMatrix);
}

void CG3DCameraModule::EnableInternal()
{
	CG3DDummyModule::EnableInternal();
	m_Scene->Set(m_Camera.get());
}
void CG3DCameraModule::DisableInternal()
{
	m_Scene->Set(NULL);
	CG3DDummyModule::DisableInternal();
}