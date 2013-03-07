#include "G3DMarmalade.h"
#include "SystemIW.h"

#include "s3e.h"
#include "s3eDebug.h"

#ifdef G3D_USE_EXTENSION
#include "s3eG3D.h"
#endif

CG3DMutexIW::CG3DMutexIW()
{
	pthread_mutex_init(&m_Mutex, NULL);
}

CG3DMutexIW::~CG3DMutexIW()
{
	pthread_mutex_destroy(&m_Mutex);
}

bool CG3DMutexIW::Lock()
{
	return pthread_mutex_lock(&m_Mutex) == 0;
}
bool CG3DMutexIW::TryLock()
{
	return pthread_mutex_trylock(&m_Mutex) == 0;
}
bool CG3DMutexIW::UnLock()
{
	return pthread_mutex_unlock(&m_Mutex) == 0;
}

void CG3DSystemIW::Trace(const char* file, int line, const char* str)
{
	s3eDebugOutputString(str);
}
int CG3DSystemIW::Error(const char* file, int line, const char* str)
{
	switch (s3eDebugErrorPrintf("%s(%d): %s", file, line, str))
	{
		case S3E_ERROR_SHOW_STOP:
			return 1;
		case S3E_ERROR_SHOW_IGNORE:
			//TODO
			break;
	}
	return 0;
}

void CG3DSystemIW::Break()
{
	//IwDebugHandleStop();
#if defined _MSC_VER && defined I3D_ARCH_X86
	do { if (s3eDebugIsDebuggerPresent()) __asm { int 3 } ; } while(0);
#endif
}

CG3DGraphics* CG3DSystemIW::Init()
{
	s_System = this;
	
#ifdef G3D_USE_EXTENSION
	if (!m_Config->m_GLOnly && s3eG3DAvailable())
		m_Setup = s3eG3DMakeExtension(this);
	else
#endif
		m_Setup = new CG3DGraphicsSetupGL();

	CG3DGraphics* graphics = m_Setup->Init(m_Config);
	if (graphics == NULL)
		return NULL;

	graphics->Init();
	return graphics;
}

CG3DSystemIW::~CG3DSystemIW()
{
	if (m_Setup != NULL)
		m_Setup->Release();
}

CG3DMutex* CG3DSystemIW::MakeMutex()
{
	return new CG3DMutexIW();
}

CG3DVector2 CG3DSystemIW::GetSize()
{
	return m_Setup->GetSize();
}

long long CG3DSystemIW::GetTime()
{
	return s3eTimerGetUST();
}

bool CG3DSystemIW::Update()
{
    s3eKeyboardUpdate();
    if (s3eDeviceCheckQuitRequest())
		return false;
    if (s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_PRESSED)
		return false;

	return true;
}
void CG3DSystemIW::EndRender()
{
	m_Setup->EndRender();
    s3eDeviceYield(0);
}

CG3DSystem* MakeG3DSystemMarmalade(CG3DSystemConfigIW* config)
{
	return new CG3DSystemIW(config);
}