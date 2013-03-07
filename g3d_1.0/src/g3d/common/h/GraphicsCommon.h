#ifndef G3D_COMMON_GRAPHICS_H
#define G3D_COMMON_GRAPHICS_H

#include "G3DCore.h"

#include <map>
#include <string>

class CG3DGraphicsCommon : public CG3DGraphics
{
protected:
	class Stack {
	public:
		CG3DGlobalSet* m_Value;
		std::vector<CG3DGlobalSet*> m_Stack;
	};
	class Param {
	public:
		float* m_FValue;
		int* m_IValue;
	public:
		Param() : m_FValue(NULL), m_IValue(NULL) {}
		Param(float* f) : m_FValue(f), m_IValue(NULL) {}
		Param(int* i) : m_FValue(NULL), m_IValue(i) {}
	};
public:
	// Global sets are sub sets of the global state
	// push a global set onto its stack and apply it
	virtual void PushGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set);
	// pop the current set and release it then push this set and apply it
	virtual void SetGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set);
	// pop the current set release it, then apply the previous set
	virtual bool PopGlobalSet(CG3DGlobalSet::SetType type);
	// get the current set
	virtual CG3DGlobalSet* GetGlobalSet(CG3DGlobalSet::SetType type);

protected:
	virtual ~CG3DGraphicsCommon();

protected:
	Stack m_Stacks[CG3DGlobalSet::MAX];
	std::map<std::string, Param> m_Params;
};
#endif