#include "2dobject.h"

void CG2DObject::Add(CG2DObject* child)
{
	m_Children.push_back(child);
	child->m_Parent = this;
}

void CG2DObject::Remove(CG2DObject* child)
{
	for (int i=0; i<(int)m_Children.size(); i++)
		if (m_Children[i] == child)
		{
			child->m_Parent = NULL;
			m_Children.erase(m_Children.begin() + i);
			break;
		}
}

void CG2DObject::Set(CG3DLayoutBase* layout)
{
	m_Layout = layout;
}