#include "SystemWin32.h"

extern void main();

CG3DSystem* MakeG3DSystemWin32()
{
	return new CG3DSystemWin32();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	CG3DSystemWin32::hInstance = hInstance;
	CG3DSystemWin32::nCmdShow = nCmdShow;

	main();

	return 0;
}