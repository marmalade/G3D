#ifndef IW_G3D_WIN32_SYSTEM_H
#define IW_G3D_WIN32_SYSTEM_H

#include "G3DCore.h"
#include "window.h"
#include "G3DDXGI.h"

class CG3DSystemWin32 : public CG3DSystem
{
public:
	virtual void Init(CG3DSystemConfig* config = NULL);

	virtual CG3DVector2 GetSize();
	virtual long long GetTime();
	virtual bool Update();
	virtual void EndRender();
	
	virtual void Trace(const char* file, int line, const char* str);
	virtual int Error(const char* file, int line, const char* str);
	virtual void Break();
	virtual void* GetInterface(const char* name);
public:
	CG3DWindow m_Window;
	CG3DDXGI m_DXGI;
public:
	static HINSTANCE hInstance;
	static int nCmdShow;
};

#endif