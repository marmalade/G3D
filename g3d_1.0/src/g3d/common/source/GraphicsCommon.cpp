#include "GraphicsCommon.h"

void CG3DGraphicsCommon::PushGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set)
{
	set->AddRef();
	m_Stacks[type].m_Stack.push_back(m_Stacks[type].m_Value);
	m_Stacks[type].m_Value = set;

	ApplySet(type);
}
void CG3DGraphicsCommon::SetGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set)
{
	set->AddRef();
	if (m_Stacks[type].m_Stack.empty())
		m_Stacks[type].m_Stack.push_back(m_Stacks[type].m_Value);
	else
		m_Stacks[type].m_Value->Release();
	m_Stacks[type].m_Value = set;

	ApplySet(type);
}
bool CG3DGraphicsCommon::PopGlobalSet(CG3DGlobalSet::SetType type)
{
	if (m_Stacks[type].m_Stack.empty())
		return false;
	
	m_Stacks[type].m_Value->Release();
	m_Stacks[type].m_Value = m_Stacks[type].m_Stack.back();
	m_Stacks[type].m_Stack.pop_back();

	ApplySet(type);
	return true;
}

CG3DGlobalSet* CG3DGraphicsCommon::GetGlobalSet(CG3DGlobalSet::SetType type)
{
	return m_Stacks[type].m_Value;
}

CG3DGraphicsCommon::~CG3DGraphicsCommon()
{
	for (int i=0; i<CG3DGlobalSet::MAX; i++) {
		for(int j=0; j<(int)m_Stacks[i].m_Stack.size(); j++)
			m_Stacks[i].m_Stack[j]->Release();
		m_Stacks[i].m_Value->Release();
	}
}