#ifndef G3D_HELPER_APP_H
#define G3D_HELPER_APP_H

// Optional class to handle the initialisation, shutdown and run loop

#include "G3DCore.h"

class CG3DApp
{
public:
	CG3DApp(CG3DSystem* system) : m_System(system) {}

	// call this
	void Run();

	// or call these
	void Init();
	virtual bool RunOne();   // returns false if quit is signaled, override to change frequency of updates per render
	void Shutdown();
protected:
	bool Update(float timeStep);
	void Render(float timeStep);

	// implement these
	virtual void InitInternal() = 0;
	virtual void ShutdownInternal() = 0;
	virtual bool UpdateInternal(float timeStep) = 0;  // return false to signal quit
	virtual void RenderInternal(float timeStep) = 0;
	virtual void SizeChanged(CG3DVector2 size);		  // screen size changed, by default adjust the viewport
protected:
	CG3DPtr<CG3DGraphics> m_Graphics;
	CG3DPtr<CG3DSystem> m_System;
	long long m_LastTime;
	CG3DPtr<CG3DViewportSet> m_Viewport;
	CG3DVector2 m_Size;
};

#endif