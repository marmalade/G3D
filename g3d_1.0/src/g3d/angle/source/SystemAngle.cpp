#include "IwG3DAngle.h"
#include "IwG3DHelper.h"

#include <Windows.h>

void CIwG3DSystemAngle::TraceInternal(const char* str)
{
	OutputDebugString(str);
}

void CIwG3DSystemAngle::ErrorInternal(const char* str)
{
	MessageBox(NULL, str, "Error", MB_OK);
}

CIwG3DSystemAngle::CIwG3DSystemAngle(void* window,int w,int h) : m_Display(EGL_NO_DISPLAY),m_Surface(EGL_NO_SURFACE),m_Context(EGL_NO_CONTEXT)
{
   m_Display=eglGetDisplay(GetDC((HWND)window));
   if(m_Display==EGL_NO_DISPLAY) return;

   int maj,min;
   if(!eglInitialize(m_Display,&maj,&min)) return;
   
   int num;
   if(!eglGetConfigs(m_Display,NULL,0,&num)) return;
   
   EGLint configAttribList[]={
       EGL_RED_SIZE,       8,
       EGL_GREEN_SIZE,     8,
       EGL_BLUE_SIZE,      8,
       EGL_DEPTH_SIZE,     24,
       EGL_NONE
   };
   EGLConfig config;
   if(!eglChooseConfig(m_Display,configAttribList,&config,1,&num)) return;

   m_Surface=eglCreateWindowSurface(m_Display,config,(HWND)window,NULL);
   if(m_Surface==EGL_NO_SURFACE) return;
   
   int contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
   m_Context=eglCreateContext(m_Display,config,EGL_NO_CONTEXT,contextAttribs);
   if(m_Context==EGL_NO_CONTEXT) return;

   eglMakeCurrent(m_Display,m_Surface,m_Surface,m_Context);
}

CIwG3DSystemAngle::~CIwG3DSystemAngle()
{
	if(m_Context!=EGL_NO_CONTEXT)
		eglDestroyContext(m_Display,m_Context);
	if(m_Surface!=EGL_NO_SURFACE)
		eglDestroySurface(m_Display,m_Surface);
    if(m_Display!=EGL_NO_DISPLAY)
		eglTerminate(m_Display);
}

void CIwG3DSystemAngle::Init()
{
}

CG3DVector2 CIwG3DSystemAngle::GetSize()
{
	return m_Size;
}

long long CIwG3DSystemAngle::GetTime()
{
	return GetTickCount();
}

bool CIwG3DSystemAngle::Update()
{
	return true;
}
void CIwG3DSystemAngle::EndRender()
{
   eglSwapBuffers(m_Display,m_Surface);
}