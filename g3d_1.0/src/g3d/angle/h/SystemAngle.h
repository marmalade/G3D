#ifndef IW_G3D_ANGLE_SYSTEM_H
#define IW_G3D_ANGLE_SYSTEM_H

#include "IwG3DCore.h"
#include "G3DGL.h"

class CIwG3DSystemAngle : public CIwG3DSystem
{
public:
	CIwG3DSystemAngle(void* window,int w,int h);
	~CIwG3DSystemAngle();
	virtual void Init();

	virtual CG3DVector2 GetSize();
	virtual long long GetTime();
	virtual bool Update();
	virtual void EndRender();
protected:
	virtual void TraceInternal(const char* str);
	virtual void ErrorInternal(const char* str);
private:
	void GetConfigDetails(EGLConfig config);
public:
	CG3DVector2 m_Size;
private:
	EGLDisplay m_Display;
	EGLSurface m_Surface;
	EGLContext m_Context;
};

#endif