#ifndef G3D_IW_H
#define G3D_IW_H

// This is an implementation of system using Marmalade

#include "G3DCore.h"
#include "Texture.h"

struct CG3DSystemConfigIW
{
	ImageFormat m_ViewFormat;	// format of the back buffer
	ImageFormat m_DepthFormat;	// format of the depth buffer
	bool m_GLOnly;				// if false the native system (like DX11) will be used on platforms that support it

	CG3DSystemConfigIW() : m_GLOnly(true) {}
};

//used by the backend
class CG3DGraphicsSetup : public CG3DBase {
public:
	virtual CG3DGraphics* Init(CG3DSystemConfigIW* config) = 0;
	virtual CG3DVector2 GetSize() = 0;
	virtual void EndRender() = 0;
};

// Call this to make the implementation for Marmalade
// optionally pass in a CG3DSystemConfigIW to configure the graphics system
CG3DSystem* MakeG3DSystemMarmalade(CG3DSystemConfigIW* config = NULL);

#endif