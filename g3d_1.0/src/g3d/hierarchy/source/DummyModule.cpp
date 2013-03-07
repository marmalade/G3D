#include "DummyModule.h"

void CG3DDummyModule::DoEvent(CG3DEvent* event)
{
	if(event->m_Event == G3D_SET_LOCAL_EVENT)
		SetLocal(((CG3DSetLocalEvent*)event)->m_Local);
}

void CG3DDummyModule::SetLocal(CG3DMatrix local)
{
	if (m_Parent->m_Parent != NULL)
		m_Parent->m_WorldMatrix = m_Parent->m_Parent->m_WorldMatrix * local;
	else
		m_Parent->m_WorldMatrix = local;
}