#ifndef IW_G3D_WIN32_DXGI_H
#define IW_G3D_WIN32_DXGI_H

#include "G3DCore.h"

#include <dxgi.h>
#include <d3d11.h>

#define DX_CHECK(error) G3D_ASSERT_RETF(SUCCEEDED(hr), ("DX Error: %s", error))
#define DX_CHECKC(error, code) G3D_ASSERT_RETFC(SUCCEEDED(hr), ("DX Error: %s", error), code)

struct CDX11 {
	CDX11() : device(NULL), deviceContext(NULL), swapChain(NULL), renderView(NULL), depthView(NULL) {}
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderView;
	ID3D11DepthStencilView* depthView;
	D3D_FEATURE_LEVEL level;
};

class CG3DDXGI
{
public:
	CG3DDXGI() : m_GotFSMode(false), m_Config(NULL), m_xFactory(NULL), m_xAdapter(NULL), m_xOutput(NULL) {}
	~CG3DDXGI();

	bool Init(HWND hWnd, CG3DSystemConfig* config);
	void Resize();

	void Test();
private:
	bool MakeDevice(UINT flags, bool error);
	bool GetOutput();
	void AdjustWindow(HWND hWnd);
	bool GetFSMode();
	bool MakeSwapChain(HWND hWnd);
	bool MakeViews();
	void ReleaseViews();

	bool GetFactory();
	bool EnumModes();
	void GetDepthFormat();
private:
	bool m_GotFSMode;
	DXGI_FORMAT m_DepthFormat;
	CG3DSystemConfig* m_Config;
	
	IDXGIFactory* m_xFactory;
	IDXGIAdapter* m_xAdapter;
	IDXGIOutput* m_xOutput;

public:
	DXGI_MODE_DESC m_FSMode;

	CDX11 m_DX;
};

#endif