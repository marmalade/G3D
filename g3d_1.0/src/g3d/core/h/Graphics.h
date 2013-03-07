#ifndef G3D_CORE_GRAPHICS_H
#define G3D_CORE_GRAPHICS_H

// abstract interface to the graphics system
// the appropiate implementation is returned by CG3DSystem::Init()

#include "G3DMaths.h"
#include "Base.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Effect.h"
#include "GlobalSet.h"
#include "RenderTo.h"

class CG3DGraphics : public CG3DRefCount
{
public:
	// called by system on initialisation, do not call yourself
	virtual void Init() = 0;
	virtual bool IsDX() = 0;
	
	// make components of the graphics system
	virtual CG3DVertexBuffer* MakeVertexBuffer() = 0;
	virtual CG3DIndexBuffer* MakeIndexBuffer() = 0;
	virtual CG3DEffect* MakeEffect() = 0;
	virtual CG3DTexture* MakeTexture() = 0;
	virtual CG3DRenderTo* MakeRenderTo() = 0;
	
	// Global sets are sub sets of the global state
	// push a global set onto its stack and apply it
	virtual void PushGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set) = 0;
	// pop the current set and release it then push this set and apply it
	virtual void SetGlobalSet(CG3DGlobalSet::SetType type, CG3DGlobalSet* set) = 0;
	// pop the current set release it, then apply the previous set
	virtual bool PopGlobalSet(CG3DGlobalSet::SetType type) = 0;
	// get the current set
	virtual CG3DGlobalSet* GetGlobalSet(CG3DGlobalSet::SetType type) = 0;
	// re-apply  the current set (call after changing the values on the set)
	virtual void ApplySet(CG3DGlobalSet::SetType type) = 0;

	// clear the screen using current state
	virtual void Clear() = 0;
};

#endif