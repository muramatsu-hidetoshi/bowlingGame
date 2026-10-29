/*==========================================================================

 DX11の機能　[direct3d.cpp]


---------------------------------------------------------------------------






=========================================================================*/








#include <d3d11.h>
#include "direct3d.h"
#include "debug_ostream.h"


#pragma comment (lib,"d3d11.lib")
#if defined(DEBUG) || defined(_DEBUG)

#pragma comment (lib,"DirectXTex_Debug.lib")

#else

#pragma comment (lib,"DirectXTex_Release.lib")

#endif
//#pragma comment (lib,"dxgi.lib")


static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11BlendState* g_pBlendState = nullptr; // ブレンドステート
static ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
static ID3D11Texture2D* g_pDepthStencilBuffer = nullptr;
static ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
static D3D11_TEXTURE2D_DESC g_BackBufferDesc{};
static D3D11_VIEWPORT g_Viewport{};
static ID3D11BlendState* g_BlendStateMultiply = nullptr; // ブレンドステート
static ID3D11BlendState* g_BlendStateAdd = nullptr; // ブレンドステート


//static D3D11_VIEWPORT g_Miewport[2]{ {},{} };
static ID3D11DepthStencilState* g_pDepthStencilStateDepthDisable = nullptr; // 深度ステンシルステート
static ID3D11DepthStencilState* g_pDepthStencilStateDepthEnable = nullptr; // 深度ステンシルステート
static ID3D11DepthStencilState* g_pDepthStencilStateDepthWriteEnable = nullptr; // 深度ステンシルステート

static bool configureBackBuffer();
static void releaseBackBuffer();




bool Direct3D_Initialize(HWND hWnd)
{
    // デバイスとスワップチェーンを作成し、画面描画に必要な状態を準備する。
	DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.BufferCount = 2;
	//swap_chain_desc.BufferDesc.Width = 0;
	//swap_chain_desc.BufferDesc.Height = 0;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	//swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	swap_chain_desc.OutputWindow = hWnd;


	//IDXGIFactory1* pFactory;
	//CreateDXGIFactory1(IID_PPV_ARGS(&pFactory));
	//IDXGIAdapter1* pAdapter;
	//pFactory->EnumAdapters1(1, &pAdapter);
	//pFactory->Release();
	//DXGI_ADAPTER_DESC1 desc;
	//pAdapter->GetDesc1(&desc);
	//pAdapter->Release(); 


	UINT device_flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		device_flags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&swap_chain_desc,
		&g_pSwapChain,
		&g_pDevice,
		&feature_level,
		&g_pDeviceContext);

	if (FAILED(hr))
	{
		MessageBox(hWnd, "DirectX3D", "Error", MB_OK);
		return false;
	}
	if (!configureBackBuffer())
	{
		MessageBox(hWnd, "DirectX3D setting", "Error", MB_OK);
		return false;
	}



	// ブレンドステート設定
	D3D11_BLEND_DESC bd = {};
	//bd.AlphaToCoverageEnable = FALSE;
	bd.AlphaToCoverageEnable = TRUE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = FALSE;
	//bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_pDevice->CreateBlendState(&bd, &g_BlendStateMultiply);
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_pDevice->CreateBlendState(&bd, &g_BlendStateAdd);

	///ここにさしこみ
     Direct3D_SetAlphaBlend(BLEND_TRANSPARENT);


	 // 共通設定
	 D3D11_DEPTH_STENCIL_DESC dsd{};
	 dsd.StencilEnable = FALSE;
	 dsd.DepthFunc = D3D11_COMPARISON_LESS;

	 // ----------------------------
	 // 深度テスト無効
	 // ----------------------------
	 dsd.DepthEnable = FALSE;
	 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	 g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthDisable);

	 // ----------------------------
	 // 深度テストON / 書き込みOFF
	 // ----------------------------
	 dsd.DepthEnable = TRUE;
	 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	 g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthEnable);

	 // ----------------------------
	 // 深度テストON / 書き込みON
	 // ----------------------------
	 dsd.DepthEnable = TRUE;
	 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	 g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthWriteEnable);

	 // 初期状態
	 g_pDeviceContext->OMSetDepthStencilState(g_pDepthStencilStateDepthEnable, 0);

	 Direct3D_SetDepthTest(true);
	return true;
}

void Direct3D_Finalize()
{

	SAFE_RELEASE(g_pDepthStencilStateDepthWriteEnable);
	SAFE_RELEASE(g_pDepthStencilStateDepthEnable);
	SAFE_RELEASE(g_pDepthStencilStateDepthDisable);
	SAFE_RELEASE(g_BlendStateMultiply);
	SAFE_RELEASE(g_pBlendState);
	releaseBackBuffer();
	SAFE_RELEASE(g_pSwapChain);
	SAFE_RELEASE(g_pDeviceContext);
	SAFE_RELEASE(g_pDevice);
	/*if (g_BlendStateMultiply)
	{
		g_BlendStateMultiply->Release();
		g_BlendStateMultiply = nullptr;
	}

	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}

	if (g_pDeviceContext)
	{
		g_pDeviceContext->Release();
		g_pDeviceContext = nullptr;
	}
	if (g_pDevice)
	{
		g_pDevice->Release();
		g_pDevice = nullptr;
	}*/
}

void Direct3D_Clear()
{
    // フレーム開始時にバックバッファと深度バッファを初期化する。
	if (!g_pDeviceContext || !g_pRenderTargetView) {
		hal::dout << "DeviceContext or RenderTargetView is null!";
		return;
	}

	float clear_color[4] = { 0.9f,1.f,0.9f,1.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, clear_color);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView,g_pDepthStencilView);

}

void Direct3D_Present()
{
    // 描画済みのバックバッファを画面へ提示する。
	g_pSwapChain->Present(1, 0);//左を1に戻す
}

unsigned int Direct3D_GetBackBufferWidth()
{

	return g_BackBufferDesc.Width;
}

unsigned int Direct3D_GetBackBufferHeight()
{
	return g_BackBufferDesc.Height;
}

ID3D11Device* Direct3D_GetDevice()
{
	return g_pDevice;
}

ID3D11DeviceContext* Direct3D_GetContext()
{
	return g_pDeviceContext;
}

void Direct3D_SetAlphaBlend(AlphaBlendMode blend)
{
	float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* arg{};

	switch (blend)
	{
	case BLEND_TRANSPARENT:
		arg = g_BlendStateMultiply;
		break;
	case BLEND_ADD:
		arg = g_BlendStateAdd;

		break;
	case BLEND_MAX:
		break;
	default:
		break;
	}



	g_pDeviceContext->OMSetBlendState(arg, blend_factor, 0xffffffff);



}

void Direct3D_SetDepthTest(bool bEnable)
{
	auto state = bEnable ? g_pDepthStencilStateDepthEnable : g_pDepthStencilStateDepthDisable;
	g_pDeviceContext->OMSetDepthStencilState(state, NULL);

}

void Direct3D_SetDepthWriteDisable()
{
	g_pDeviceContext->OMSetDepthStencilState(
		g_pDepthStencilStateDepthEnable, 0);
}

void Direct3D_SetDepthWriteEnable()
{
	g_pDeviceContext->OMSetDepthStencilState(
		g_pDepthStencilStateDepthWriteEnable, 0);
}

//void SetViewport(int n)
//{
//	g_pDeviceContext->RSSetViewports(1, &g_Viewport[n]);
//}

bool configureBackBuffer()
{
	HRESULT hr;

	ID3D11Texture2D* back_buffer_pointer = nullptr;

	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_pointer);

	if (FAILED(hr))
	{
		hal::dout << "back buffer fail" ;
		return false;
	}

	hr = g_pDevice->CreateRenderTargetView(back_buffer_pointer, nullptr, &g_pRenderTargetView);

	if (FAILED(hr))
	{
		back_buffer_pointer->Release();
		hal::dout << "back buffer RenderTargetView fail";
		return false;
	}

	back_buffer_pointer->GetDesc(&g_BackBufferDesc);

	back_buffer_pointer->Release();

	D3D11_TEXTURE2D_DESC depth_stencil_desc{};
	depth_stencil_desc.Width = g_BackBufferDesc.Width;
	depth_stencil_desc.Height = g_BackBufferDesc.Height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_desc.SampleDesc.Count = 1;
	depth_stencil_desc.SampleDesc.Quality = 0;
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = g_pDevice->CreateTexture2D(&depth_stencil_desc, nullptr, &g_pDepthStencilBuffer);

	if (FAILED(hr))
	{
		hal::dout << "depth stenvil desc fail" ;
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
	depth_stencil_view_desc.Format = depth_stencil_desc.Format;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilBuffer, &depth_stencil_view_desc, &g_pDepthStencilView);

	if (FAILED(hr))
	{
		hal::dout << "depth stencil view desc fail" ;
		return false;
	}


	g_Viewport.TopLeftX = 0.0f;
	g_Viewport.TopLeftY = 0.0f;
	g_Viewport.Width = (FLOAT)g_BackBufferDesc.Width;
	g_Viewport.Height = (FLOAT)g_BackBufferDesc.Height;
	g_Viewport.MinDepth = 0.0f;
	g_Viewport.MaxDepth = 1.0f;


	/*g_Viewport[0].TopLeftX = 0.0f;
	g_Viewport[0].TopLeftY = 0.0f;
	g_Viewport[0].Width = (FLOAT)g_BackBufferDesc.Width*0.5;
	g_Viewport[0].Height = (FLOAT)g_BackBufferDesc.Height;
	g_Viewport[0].MinDepth = 0.0f;
	g_Viewport[0].MaxDepth = 1.0f;

	g_Viewport[1].TopLeftX = (FLOAT)g_BackBufferDesc.Width * 0.5;
	g_Viewport[1].TopLeftY = 0.0f;
	g_Viewport[1].Width = (FLOAT)g_BackBufferDesc.Width*0.5;
	g_Viewport[1].Height = (FLOAT)g_BackBufferDesc.Height;
	g_Viewport[1].MinDepth = 0.0f;
	g_Viewport[1].MaxDepth = 1.0f;*/


	g_pDeviceContext->RSSetViewports(1, &g_Viewport);


	return true;
}

void releaseBackBuffer()
{
	if (g_pRenderTargetView)
	{
		g_pRenderTargetView->Release();
		g_pRenderTargetView = nullptr;
	}

	if (g_pDepthStencilBuffer)
	{
		g_pDepthStencilBuffer->Release();
		g_pDepthStencilBuffer = nullptr;
	}
	if (g_pDepthStencilView)
	{
		g_pDepthStencilView->Release();
		g_pDepthStencilView = nullptr;
	}

	if (g_BlendStateMultiply)
	{
		g_BlendStateMultiply->Release();
		g_BlendStateMultiply = nullptr;
	}

	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = nullptr;
	}

	if (g_pDeviceContext)
	{
		g_pDeviceContext->Release();
		g_pDeviceContext = nullptr;
	}
	if (g_pDevice)
	{
		g_pDevice->Release();
		g_pDevice = nullptr;
	}
}