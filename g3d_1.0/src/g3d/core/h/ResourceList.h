#ifndef G3D_CORE_RESOURCE_LIST_H
#define G3D_CORE_RESOURCE_LIST_H

// List of resources
// This class deals with reference counting, Upload(), Unload() and Apply() for all its children.

#include "Resource.h"
#include <vector>

template<class T>
class CG3DResourceList : public CG3DResource {
public:
	virtual void Add(T* item)
	{
		item->AddRef();
		m_List.push_back(item);

		if (GetInternalFlag(UPLOADED))
		{
			m_List.back()->Upload();
			AddInternal(m_List.back());
		}
	}
	virtual void Remove(T* item)
	{
		for (int i=0; i<(int)m_List.size(); i++)
			if (m_List[i] == item)
			{
				if (GetInternalFlag(UPLOADED))
				{
					RemoveInternal(m_List[i]);
					m_List[i]->Unload();
				}

				m_List.erase(m_List.begin() + i);
				item->Release();
				return;
			}
	}
	virtual void Upload()
	{
		for (int i=0; i<(int)m_List.size(); i++)
		{
			m_List[i]->Upload();
			if (!GetInternalFlag(UPLOADED))
				AddInternal(m_List[i]);
		}

		CG3DResource::Upload();
	}
	virtual void Reload()
	{
		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->Reload();

		CG3DResource::Reload();
	}
	virtual void Unload()
	{
		for (int i=0; i<(int)m_List.size(); i++)
		{
			if(GetInternalFlag(UPLOADED))
				RemoveInternal(m_List[i]);
			m_List[i]->Unload();
		}
		CG3DResource::Unload();
	}
	virtual void Apply()
	{
		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->Apply();

		CG3DResource::Apply();
	}
	virtual void Release()
	{
		m_RefCount--;
		if(m_RefCount>0)
			return;

		if (GetInternalFlag(UPLOADED))
			Unload();

		for (int i=0; i<(int)m_List.size(); i++)
			m_List[i]->Release();

		ReleaseInternal();
		CG3DBase::Release();
	}
protected:
	virtual void AddInternal(T* item) = 0;
	virtual void RemoveInternal(T* item) = 0;
protected:
	std::vector<T*> m_List;
};

#endif