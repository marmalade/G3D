#include "Module.h"
#include "GameObject.h"

void CG3DModule::Enable()
{
	if(m_Enabled) return;
	EnableInternal();
	m_Enabled=true;
}
void CG3DModule::Disable()
{
	if(!m_Enabled) return;
	DisableInternal();
	m_Enabled=false;
}

void CG3DModule::Release()
{
	Disable();
	CG3DBase::Release();
}