#ifndef IW_G3D_WIN32_WINDOW_H
#define IW_G3D_WIN32_WINDOW_H

#include <Windows.h>

#include "G3DCore.h"
#include "G3DDXGI.h"

class CG3DWindow
{
public:
	bool Init(HINSTANCE hInstance, CG3DSystemConfig* config);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool Messages();
public:
	HWND m_hWnd;
	CG3DDXGI* m_dxgi;
};

#endif