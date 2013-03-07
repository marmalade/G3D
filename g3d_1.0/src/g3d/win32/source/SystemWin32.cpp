#include "SystemWin32.h"

HINSTANCE CG3DSystemWin32::hInstance = NULL;
int CG3DSystemWin32::nCmdShow = 0;

void CG3DSystemWin32::Trace(const char* file, int line, const char* str)
{
	OutputDebugStringA(str);
}

int CG3DSystemWin32::Error(const char* file, int line, const char* str)
{
	return _CrtDbgReport(_CRT_ASSERT, file, line, NULL, str);
}

void CG3DSystemWin32::Init(CG3DSystemConfig* config)
{
	s_System = this;
	m_Window.m_dxgi = &m_DXGI;

	CG3DSystemConfig defaultConfig = {640, 480, true, 0};
	if (config == NULL)
	{
		config = &defaultConfig;
	}
	if (!m_Window.Init(hInstance, config))
		return;
	if (!m_DXGI.Init(m_Window.m_hWnd, config))
		return;

    ShowWindow(m_Window.m_hWnd, nCmdShow);
}

void* CG3DSystemWin32::GetInterface(const char* name)
{
	if (!strcmp(name, "DX11"))
		return &m_DXGI.m_DX;
	return NULL;
}

void CG3DSystemWin32::Break()
{
	_CrtDbgBreak();
}

CG3DVector2 CG3DSystemWin32::GetSize()
{
	return CG3DVector2((float)m_DXGI.m_FSMode.Width, (float)m_DXGI.m_FSMode.Height);
}

long long CG3DSystemWin32::GetTime()
{
	return GetTickCount();
}

bool CG3DSystemWin32::Update()
{
	return m_Window.Messages();
}
void CG3DSystemWin32::EndRender()
{
	m_DXGI.Test();
}