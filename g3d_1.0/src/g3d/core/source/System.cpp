#define _CRT_SECURE_NO_WARNINGS
#pragma unmanaged

#include "System.h"
#include <stdarg.h>
#include <stdio.h>

CG3DSystem* CG3DSystem::s_System = NULL;

const char* CG3DSystem::Format(const char* format, ...)
{
	va_list args;
	static char buf[256];

	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	return buf;
}

CG3DSystem* CG3DSystem::GetSystem()
{
	return s_System;
}
