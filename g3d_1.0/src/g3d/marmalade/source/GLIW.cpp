#include "G3DMarmalade.h"
#include "G3DGLES.h"
#include "SystemIW.h"

#ifdef G3D_USE_IWGL
#include <IwGL.h>
#else
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#endif
#include "s3e.h"
#include "s3eDebug.h"

#include <map>
#include <vector>
#include <stdio.h>

#ifndef G3D_USE_IWGL
static EGLSurface g_EGLSurface = NULL;
static EGLDisplay g_EGLDisplay = NULL;
static EGLContext g_EGLContext = NULL;
#endif

#define MAX_CONFIG 32

CG3DGraphics* CG3DGraphicsSetupGL::Init(CG3DSystemConfigIW* systemConfig)
{
#ifdef G3D_USE_IWGL
	IwGLInit();
#else
    g_EGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!g_EGLDisplay)
    {
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglGetDisplay failed");
        return NULL;
    }

    EGLBoolean res = eglInitialize(g_EGLDisplay, NULL, NULL);
    if (!res)
    {
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglInitialize failed");
        return NULL;
    }
	
    EGLint numFound = 0;
    EGLConfig configList[MAX_CONFIG];
    eglGetConfigs(g_EGLDisplay, configList, MAX_CONFIG, &numFound);
    if (!numFound)
    {
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglGetConfigs failed to find any configs");
        return NULL;
    }

    int config = -1;
	s3eDebugTracePrintf("found %d configs\n", numFound);
	for (int i=0; i<numFound; i++) {
		int configDesc[8];
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_COLOR_BUFFER_TYPE, &configDesc[0]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_RED_SIZE, &configDesc[1]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_GREEN_SIZE, &configDesc[2]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_BLUE_SIZE, &configDesc[3]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_ALPHA_SIZE, &configDesc[4]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_LUMINANCE_SIZE, &configDesc[5]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_DEPTH_SIZE, &configDesc[6]);
		eglGetConfigAttrib(g_EGLDisplay, configList[i], EGL_STENCIL_SIZE, &configDesc[7]);
		
		if (configDesc[0] == EGL_RGB_BUFFER)
			s3eDebugTracePrintf("  config %d: r:%d g:%d b:%d a:%d d:%d s:%d\n", i, configDesc[1], configDesc[2], configDesc[3], configDesc[4], configDesc[6], configDesc[7]);
		else
			s3eDebugTracePrintf("  config %d: l:%d a:%d d:%d s:%d\n", i, configDesc[5], configDesc[4], configDesc[6], configDesc[7]);
	}
	
	int attribList[] = {
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,			8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE,			8,
		EGL_LUMINANCE_SIZE,		0,
		EGL_DEPTH_SIZE,			8,
		EGL_STENCIL_SIZE,		0,
		EGL_NONE};
	int attribList2[] = {
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,			8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE,			8,
		EGL_LUMINANCE_SIZE,		0,
		EGL_DEPTH_SIZE,			8,
		EGL_STENCIL_SIZE,		0,
		EGL_NONE};
	
	if (systemConfig != NULL) 
	{
		switch (systemConfig->m_ViewFormat.depth)
		{
			case ImageFormat::D32:
				attribList[3] = 32; attribList[5] = 32; attribList[7] = 32; attribList[9] = 32; break;
			case ImageFormat::D11_11_10:
				attribList[3] = 11; attribList[5] = 11; attribList[7] = 10; attribList[9] = 0; break;
			case ImageFormat::D10_10_10_2:
				attribList[3] = 10; attribList[5] = 10; attribList[7] = 10; attribList[9] = 2; break;
			case ImageFormat::D16:
				attribList[3] = 16; attribList[5] = 16; attribList[7] = 16; attribList[9] = 16; break;
			case ImageFormat::D5_6_5:
				attribList[3] = 5; attribList[5] = 6; attribList[7] = 5; attribList[9] = 0; break;
			case ImageFormat::D5_5_5_1:
				attribList[3] = 5; attribList[5] = 5; attribList[7] = 5; attribList[9] = 1; break;
			case ImageFormat::D4_4_4_4:
				attribList[3] = 4; attribList[5] = 4; attribList[7] = 4; attribList[9] = 4; break;
		}
		switch (systemConfig->m_ViewFormat.layout)
		{
			case ImageFormat::RGB: case ImageFormat::BGR: case ImageFormat::BGRX:
				attribList[9] = 0; break;
			case ImageFormat::L:
				attribList[1] = EGL_LUMINANCE_BUFFER; attribList[11] = attribList[3];
				attribList[3] = 0; attribList[5] = 0; attribList[7] = 0; attribList[9] = 0; break;
			case ImageFormat::LA:
				attribList[1] = EGL_LUMINANCE_BUFFER; attribList[11] = attribList[3];
				attribList[3] = 0; attribList[5] = 0; attribList[7] = 0; break;
		}
		switch (systemConfig->m_DepthFormat.depth)
		{
			case ImageFormat::D32_8_24:
				attribList[13] = 32; attribList[15] = 8; break;
			case ImageFormat::D32:
				attribList[13] = 32; attribList[15] = 0; break;
			case ImageFormat::D24_8:
				attribList[13] = 24; attribList[15] = 8; break;
			case ImageFormat::D16:
				attribList[13] = 16; attribList[15] = 0; break;
		}
	}

	eglChooseConfig(g_EGLDisplay, attribList, configList, numFound, &config);
	
    if (config == -1)
		eglChooseConfig(g_EGLDisplay, attribList2, configList, numFound, &config);

    if (config == -1)
    {
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "No GLES2 configs reported.  Trying random config");
        config = 0;
    }
	s3eDebugTracePrintf("chosen config %d\n", config);

    int version = s3eGLGetInt(S3E_GL_VERSION)>>8;

    EGLint attribs[] = { EGL_CONTEXT_CLIENT_VERSION, version, EGL_NONE, };
    g_EGLContext = eglCreateContext(g_EGLDisplay, configList[config], NULL, attribs);
    if (!g_EGLContext)
    {
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "eglCreateContext failed");
        return NULL;
    }

    version = s3eGLGetInt(S3E_GL_VERSION)>>8;
    if (version != 2)
    {
        printf("reported GL version: %d", version);
        s3eDebugErrorShow(S3E_MESSAGE_CONTINUE, "This example requires GLES v2.x");
        return NULL;
    }

    void* nativeWindow = s3eGLGetNativeWindow();
    g_EGLSurface = eglCreateWindowSurface(g_EGLDisplay, configList[config], nativeWindow, NULL);
    eglMakeCurrent(g_EGLDisplay, g_EGLSurface, g_EGLSurface, g_EGLContext);
#endif
	
	return MakeG3DGraphicsGLES();
}

CG3DGraphicsSetupGL::~CG3DGraphicsSetupGL()
{
#ifdef G3D_USE_IWGL
	IwGLTerminate();
#else
    if (g_EGLDisplay)
    {
        eglMakeCurrent(g_EGLDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(g_EGLDisplay, g_EGLSurface);
        eglDestroyContext(g_EGLDisplay, g_EGLContext);
    }
    eglTerminate(g_EGLDisplay);
    g_EGLDisplay = 0;
#endif
}

CG3DVector2 CG3DGraphicsSetupGL::GetSize()
{
#ifdef G3D_USE_IWGL
	return CG3DVector2((float)IwGLGetInt(IW_GL_WIDTH), (float)IwGLGetInt(IW_GL_HEIGHT));
#else
	return CG3DVector2((float)s3eSurfaceGetInt(S3E_SURFACE_WIDTH), (float)s3eSurfaceGetInt(S3E_SURFACE_HEIGHT));
#endif
}

void CG3DGraphicsSetupGL::EndRender()
{
#ifdef G3D_USE_IWGL
    IwGLSwapBuffers();
#else
    eglSwapBuffers(g_EGLDisplay, g_EGLSurface);
#endif
}