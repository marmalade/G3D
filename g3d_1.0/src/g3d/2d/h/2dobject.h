#ifndef G3D_2D_OBJECT_H
#define G3D_2D_OBJECT_H

// Optional class to do 2D graphics

#include "G3DCore.h"
#include <vector>

class CG2DObject;

class CG3DLayoutBase
{
public:
	void Init(CG2DObject* object) { m_Object = object; }
protected:
	CG2DObject* m_Object;
};

class CG3DDrawBase : CG3DResource
{
public:
	void Init(CG2DObject* object) { m_Object = object; }
protected:
	CG2DObject* m_Object;
};

class CG2DObject
{
public:
	CG2DObject() : m_Parent(NULL), m_Local(CG3DMatrix33::s_Identity), m_Layout(NULL) {}

	void Add(CG2DObject* child);
	void Remove(CG2DObject* child);

	void Set(CG3DLayoutBase* layout);

	unsigned int NumChildren() { return m_Children.size(); }
	template<class T>
	T* Get(unsigned int index) { return index>=m_Children.size() ? NULL : dynamic_cast<T*>(m_Children[i]); }

public:
	CG3DMatrix33 m_Local;
private:
	CG2DObject* m_Parent;
	std::vector<CG2DObject*> m_Children;
	CG3DLayoutBase* m_Layout;
};

#endif