#include "G3DDXGI.h"

bool CG3DDXGI::Init(HWND hWnd, CG3DSystemConfig* config)
{
	bool result = true;
	m_Config = config;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&m_xFactory));
	DX_CHECK("Failed to create factory");
	
	hr = m_xFactory->MakeWindowAssociation(hWnd, 0);
	DX_CHECKC("Failed to associate window", m_xFactory->Release());
	
	//EnumModes();
	
	if (!GetOutput())
		result = false;
	
	if(result)
		AdjustWindow(hWnd);
	
	if (result && m_Config->m_FullScreen && !GetFSMode())
		result = false;
	
	if (result && !MakeDevice(D3D11_CREATE_DEVICE_DEBUG, false))
		if (!MakeDevice(0, true))
			return false;
	
	if (result && !MakeSwapChain(hWnd))
		result = false;

	if (result)
		GetDepthFormat();

	if (result && !MakeViews())
		result = false;
	
	if (m_xOutput)
	{
		m_xOutput->Release();
	}
	if (m_xAdapter)
	{
		m_xAdapter->Release();
	}
	m_xFactory->Release();
	return true;
}

bool CG3DDXGI::GetOutput()
{
	int num = m_Config->m_Display;
	bool gotDisplay = false;
	
	IDXGIAdapter* xAdapter;
	for (int i=0; !gotDisplay; i++)
	{
		if (m_xFactory->EnumAdapters(i, &xAdapter) == DXGI_ERROR_NOT_FOUND)
			break;

		if (m_xAdapter == NULL)
		{
			xAdapter->AddRef();
			m_xAdapter = xAdapter;
		}
		
		IDXGIOutput* xOutput;
		for (int j=0; !gotDisplay; j++)
		{
			if (xAdapter->EnumOutputs(j, &xOutput) == DXGI_ERROR_NOT_FOUND)
				break;
			
			if (m_xOutput == NULL)
			{
				xOutput->AddRef();
				m_xOutput = xOutput;
			}
			if (num == 0)
			{
				xAdapter->AddRef();
				m_xAdapter->Release();
				m_xAdapter = xAdapter;
				
				xOutput->AddRef();
				m_xOutput->Release();
				m_xOutput = xOutput;
				gotDisplay = true;
			}
			num--;

			xOutput->Release();
		}
		xAdapter->Release();
	}
	G3D_ASSERT_RETF(m_xOutput != NULL, ("No displays!"));

	return true;
}

void CG3DDXGI::AdjustWindow(HWND hWnd)
{
	RECT winRect;
	DXGI_OUTPUT_DESC desc;

	GetWindowRect(hWnd, &winRect);
	m_xOutput->GetDesc(&desc);

	if (winRect.left < desc.DesktopCoordinates.left)
	{
		winRect.left += desc.DesktopCoordinates.left;
	}
	else if (winRect.right > desc.DesktopCoordinates.right)
	{
		winRect.left += desc.DesktopCoordinates.left;
	}

	if (winRect.top < desc.DesktopCoordinates.top)
	{
		winRect.top += desc.DesktopCoordinates.top;
	}
	else if (winRect.bottom > desc.DesktopCoordinates.bottom)
	{
		winRect.top += desc.DesktopCoordinates.top;
	}
	SetWindowPos(hWnd, NULL, winRect.left, winRect.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

bool CG3DDXGI::GetFSMode()
{
	DXGI_FORMAT formats[] = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
		DXGI_FORMAT_B5G6R5_UNORM,
		DXGI_FORMAT_R10G10B10A2_UNORM,
		DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_UNKNOWN,
	};

	for (int k=0; formats[k]!=DXGI_FORMAT_UNKNOWN; k++)
	{		
		DXGI_MODE_DESC desired;
		desired.Width = m_Config->m_Width;
		desired.Height = m_Config->m_Height;
		desired.Format = formats[k];
		desired.RefreshRate.Numerator = 0;
		desired.RefreshRate.Denominator = 0;
		desired.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desired.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		HRESULT hr = m_xOutput->FindClosestMatchingMode(&desired, &m_FSMode, NULL);
		if(hr == S_OK)
		{
			m_GotFSMode = true;
			return true;
		}
	}
	return false;
}

bool CG3DDXGI::MakeDevice(UINT flags, bool error)
{
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	HRESULT hr = D3D11CreateDevice(m_xAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 
		flags, levels, 6, D3D11_SDK_VERSION, &m_DX.device, &m_DX.level, &m_DX.deviceContext);
	
	if (!error)
		DX_CHECK("Failed to create device");
	return SUCCEEDED(hr);
}

bool CG3DDXGI::MakeSwapChain(HWND hWnd)
{
	if (!m_GotFSMode)
		m_Config->m_FullScreen = false;

	DXGI_SWAP_CHAIN_DESC desc;
	desc.Windowed = !m_Config->m_FullScreen;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.OutputWindow = hWnd;

	if (m_Config->m_FullScreen)
	{
		memcpy(&desc.BufferDesc, &m_FSMode, sizeof(DXGI_MODE_DESC));
	}
	else
	{
		desc.BufferDesc.Width = m_Config->m_Width;
		desc.BufferDesc.Height = m_Config->m_Height;
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 0;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		
		memcpy(&m_FSMode, &desc.BufferDesc, sizeof(DXGI_MODE_DESC));
	}

	HRESULT hr = m_xFactory->CreateSwapChain(m_DX.device, &desc, &m_DX.swapChain);
	DX_CHECK("Failed to create swapchain");

	return true;
}

bool CG3DDXGI::MakeViews()
{
	ID3D11Texture2D* buffer;
	HRESULT hr = m_DX.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
	DX_CHECK("Failed to get back buffer");

	hr = m_DX.device->CreateRenderTargetView(buffer, NULL, &m_DX.renderView);
	buffer->Release();
	DX_CHECK("Failed to make render view");
	
	m_DX.depthView = NULL;
	if (m_DepthFormat != DXGI_FORMAT_UNKNOWN)
	{
		ID3D11Texture2D* depth;
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = m_FSMode.Width;
		descDepth.Height = m_FSMode.Height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = m_DepthFormat;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = m_DX.device->CreateTexture2D(&descDepth, NULL, &depth);
		DX_CHECK("Failed to make depth stencil texture");
	
		hr = m_DX.device->CreateDepthStencilView(depth, NULL, &m_DX.depthView);
		depth->Release();
		DX_CHECK("Failed to make depth stencil view");
	}

	m_DX.deviceContext->OMSetRenderTargets(1, &m_DX.renderView, m_DX.depthView);

	return true;
}

void CG3DDXGI::ReleaseViews()
{
	if (m_DX.renderView)
		m_DX.renderView->Release();
	m_DX.renderView = NULL;

	if (m_DX.depthView)
		m_DX.depthView->Release();
	m_DX.depthView = NULL;

	m_DX.deviceContext->OMSetRenderTargets(0, NULL, NULL);
}

void CG3DDXGI::GetDepthFormat()
{
	DXGI_FORMAT format[] = {
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		DXGI_FORMAT_D32_FLOAT,
		DXGI_FORMAT_D16_UNORM,
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		DXGI_FORMAT_UNKNOWN
	};

	for (int i=0; format[i] != DXGI_FORMAT_UNKNOWN; i++)
	{
		UINT support;
		HRESULT hr = m_DX.device->CheckFormatSupport(format[i], &support);
		if (SUCCEEDED(hr) && (support & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL))
		{
			m_DepthFormat = format[i];
			return;
		}
	}
	m_DepthFormat = DXGI_FORMAT_UNKNOWN;
}

void CG3DDXGI::Resize()
{
	//if (!m_DX.renderView)
		return;

	ReleaseViews();

	m_DX.swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	MakeViews();
}

bool CG3DDXGI::EnumModes()
{	
	bool found = false;
	IDXGIAdapter* xAdapter;
	for (int i=0;!found;i++)
	{
		if (m_xFactory->EnumAdapters(i, &xAdapter) == DXGI_ERROR_NOT_FOUND)
			break;

		DXGI_ADAPTER_DESC aDesc;
		xAdapter->GetDesc(&aDesc);
		
		IDXGIOutput* xOutput;
		for (int j=0;!found;j++)
		{
			if (xAdapter->EnumOutputs(j, &xOutput) == DXGI_ERROR_NOT_FOUND)
				break;

			DXGI_OUTPUT_DESC oDesc;
			xOutput->GetDesc(&oDesc);
			
			for (int i=0; i<100; i++)
			{
				UINT num = 0;
				xOutput->GetDisplayModeList((DXGI_FORMAT)i, DXGI_ENUM_MODES_INTERLACED|DXGI_ENUM_MODES_SCALING, &num, NULL);

				if (num>0)
				{
					DXGI_MODE_DESC* modes = new DXGI_MODE_DESC[num];
					xOutput->GetDisplayModeList((DXGI_FORMAT)i, DXGI_ENUM_MODES_INTERLACED|DXGI_ENUM_MODES_SCALING, &num, modes);
					delete[] modes;
				}
			}

			xOutput->Release();
		}
		xAdapter->Release();
	}
	
	return true;
}

bool CG3DDXGI::GetFactory()
{
	IDXGIDevice *xDXGIDevice;
	HRESULT hr = m_DX.device->QueryInterface(__uuidof(IDXGIDevice), (void **)&xDXGIDevice);
	DX_CHECK("Failed to get dxgi device");
      
	IDXGIAdapter *xDXGIAdapter;
	hr = xDXGIDevice->GetAdapter(&xDXGIAdapter);
	DX_CHECK("Failed to get dxgi adapter");

	hr = xDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&m_xFactory);
	DX_CHECK("Failed to get dxgi factory");

	xDXGIDevice->Release();
	xDXGIAdapter->Release();

	return true;
}

CG3DDXGI::~CG3DDXGI()
{
	if (m_DX.depthView)
	{
		m_DX.depthView->Release();
	}
	if (m_DX.renderView)
	{
		m_DX.renderView->Release();
	}
	if (m_DX.swapChain)
	{
		m_DX.swapChain->Release();
	}
	if (m_DX.deviceContext)
	{
		m_DX.deviceContext->Release();
	}
	if (m_DX.device)
	{
		m_DX.device->Release();
	}
}

void CG3DDXGI::Test()
{
	m_DX.swapChain->Present(0, 0);
}