#include "window.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CG3DWindow* win = (CG3DWindow*)GetWindowLong(hWnd, GWL_USERDATA);

	return win->WndProc(hWnd, message, wParam, lParam);
}

LRESULT CG3DWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_SIZE:
			m_dxgi->Resize();
			break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

bool CG3DWindow::Init(HINSTANCE hInstance, CG3DSystemConfig* config)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"G3DWin32";
    wcex.hIconSm = NULL;

	DWORD result = RegisterClassEx(&wcex);
	G3D_ASSERT_RETF(result, ("Register class failed"));
	
	RECT rc = {0, 0, config->m_Width, config->m_Height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hWnd = CreateWindow(L"G3DWin32", L"G3D Win32 Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,NULL);
    G3D_ASSERT_RETF(m_hWnd != NULL, ("Create Window failed"));

	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);

	return true;
}

bool CG3DWindow::Messages()
{
    MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(WM_QUIT == msg.message)
			return false;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}