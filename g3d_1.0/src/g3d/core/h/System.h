#ifndef G3D_CORE_SYSTEM_H
#define G3D_CORE_SYSTEM_H

// This is the system interface, an interface to commonly use system functions
// This needs to be Instantated as the graphics system uses it to report errors

#include "G3DMaths.h"
#include "Base.h"
#include "Graphics.h"
#include "Mutex.h"

#include <stddef.h>

class CG3DSystem : public CG3DRefCount
{
public:
	// initialise and return graphics library
	virtual CG3DGraphics* Init() = 0;
	// get a mutex object
	virtual CG3DMutex* MakeMutex() = 0;

	// get the current screen size
	virtual CG3DVector2 GetSize() = 0;
	// get the tick count
	virtual long long GetTime() = 0;
	// update the system
	virtual bool Update() = 0;
	// swap the render buffers
	virtual void EndRender() = 0;
public:
	// report messages and errors
	virtual void Trace(const char* file, int line, const char* str) = 0;
	virtual int Error(const char* file, int line, const char* str) = 0;
	virtual void Break() = 0;

	virtual void* GetInterface(const char* name) { return NULL; }

	static const char* Format(const char* format, ...);
	static CG3DSystem* GetSystem();

	// in the extension setup the pointer
	static void SetupSystem(CG3DSystem* system) { s_System = system; }
protected:
	static CG3DSystem* s_System;
};

#ifdef G3D_DEBUG
// G3D_ASSERT_EXPR(<bool expr>, ("<message>", <params>));
#define G3D_ASSERT_EXPR(expr, msg) \
	(void) ((!!(expr)) || (1 != CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) || (CG3DSystem::GetSystem()->Break(), 0))
// G3D_ASSERT_RET(<bool expr>, ("<message>", <params>))	this returns on false
#define G3D_ASSERT_RET(expr, msg) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); return; }}
// G3D_ASSERT_RETC(<bool expr>, ("<message>", <params>), <code>)	this calls code then returns on false
#define G3D_ASSERT_RETC(expr, msg, code) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); code; return; }}
// G3D_ASSERT_RETC(<bool expr>, ("<message>", <params>), <code>)	this returns false on false
#define G3D_ASSERT_RETF(expr, msg) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); return false; }}
// G3D_ASSERT_RETC(<bool expr>, ("<message>", <params>), <code>)	this calls code then returns false on false
#define G3D_ASSERT_RETFC(expr, msg, code) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); code; return false; }}
// G3D_ASSERT_RETC(<bool expr>, ("<message>", <params>), <code>)	this returns NULL on false
#define G3D_ASSERT_RETN(expr, msg) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); return NULL; }}
// G3D_ASSERT_RETC(<bool expr>, ("<message>", <params>), <code>)	this calls code then returns NULL on false
#define G3D_ASSERT_RETNC(expr, msg, code) \
	{ if(!(expr)) { if (1 == CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, CG3DSystem::Format msg)) CG3DSystem::GetSystem()->Break(); code; return NULL; }}
#else
#define G3D_ASSERT_EXPR(expr, msg)
#define G3D_ASSERT_RET(expr, msg)
#define G3D_ASSERT_RETC(expr, msg, code)
#define G3D_ASSERT_RETF(expr, msg)
#define G3D_ASSERT_RETFC(expr, msg, code)
#define G3D_ASSERT_RETN(expr, msg)
#define G3D_ASSERT_RETNC(expr, msg, code)
#endif

#endif