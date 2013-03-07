#ifndef G3D_IW_SYSTEM_H
#define G3D_IW_SYSTEM_H

#include "G3DCore.h"
#include <pthread.h>

class CG3DMutexIW : public CG3DMutex
{
public:
	CG3DMutexIW();
	~CG3DMutexIW();

	virtual bool Lock();
	virtual bool TryLock();
	virtual bool UnLock();
private:
	pthread_mutex_t m_Mutex;
};

class CG3DGraphicsSetupGL : public CG3DGraphicsSetup {
public:
	virtual CG3DGraphics* Init(CG3DSystemConfigIW* config);
	~CG3DGraphicsSetupGL();

	virtual CG3DVector2 GetSize();
	virtual void EndRender();
};

class CG3DSystemIW : public CG3DSystem
{
public:
	CG3DSystemIW(CG3DSystemConfigIW* config) : m_Config(config), m_Setup(NULL) {}
	virtual CG3DGraphics* Init();
	~CG3DSystemIW();
	
	virtual CG3DMutex* MakeMutex();

	virtual CG3DVector2 GetSize();
	virtual long long GetTime();
	virtual bool Update();
	virtual void EndRender();

	virtual void Trace(const char* file, int line, const char* str);
	virtual int Error(const char* file, int line, const char* str);
	virtual void Break();
public:
	CG3DSystemConfigIW* m_Config;
	CG3DGraphicsSetup* m_Setup;
};

#endif