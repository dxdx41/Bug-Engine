#include "D3DRenderer.h"
#include "Util/Log.h"

D3DRenderer::~D3DRenderer()
{
}

bool D3DRenderer::Initialize(HWND hWnd) {
	Log.info("Initializing renderer...");
	this->hWnd = hWnd;

	/* create device */
	{ // scope for comptrs
		Microsoft::WRL::ComPtr<ID3D11Device> baseDevice{ nullptr };
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> baseContext{ nullptr };

		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
		#ifdef _DEBUG
			creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			baseDevice.ReleaseAndGetAddressOf(),
			nullptr,
			baseContext.ReleaseAndGetAddressOf()
		);
		if (FAILED(hr)) {
			Log.error("[Device] D3D11CreateDevice() failed");
			return false;
		}

		hr = baseDevice->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(pDevice.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) {
			Log.error("[Device] baseDevice->QueryInterface() failed");
			return false;
		}

		hr = baseContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(pContext.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) {
			Log.error("[Device] baseContext->QueryInterface() failed");
			return false;
		}
	}

	/* create swapchain */
	{
		Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory{ nullptr };
		{
			Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice{ nullptr };
			HRESULT hr = pDevice->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(dxgiDevice.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) {
				Log.error("[SwapChain] pDevice->QueryInterface() failed");
				return false;
			}

			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter{ nullptr };
			hr = dxgiDevice->GetAdapter(dxgiAdapter.ReleaseAndGetAddressOf());
			if (FAILED(hr)) {
				Log.error("[SwapChain] dxgiDevice->GetAdapter() failed");
				return false;
			}

			DXGI_ADAPTER_DESC adapterDesc{};
			dxgiAdapter->GetDesc(&adapterDesc);

			std::wstring desc_w{ adapterDesc.Description };
			Log.info("D3D11 Graphics Device: " + std::string(desc_w.begin(), desc_w.end()));

			hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(dxgiFactory.ReleaseAndGetAddressOf()));
			if (FAILED(hr)) {
				Log.error("[SwapChain] dxgiAdapter->GetParent() failed");
				return false;
			}
		}

		DXGI_SWAP_CHAIN_DESC1 sd{};
		RECT clientRect{};
		GetClientRect(hWnd, &clientRect);
		sd.Width = clientRect.right - clientRect.left;
		sd.Height = clientRect.bottom - clientRect.top;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Stereo = FALSE;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 2;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		sd.Flags = NULL;

		HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(pDevice.Get(), hWnd, &sd, nullptr, nullptr, pSwapChain.ReleaseAndGetAddressOf());
	}

	/* create render target */
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pFrameBuffer{ nullptr };
		HRESULT hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pFrameBuffer.ReleaseAndGetAddressOf()));
		if (FAILED(hr)) {
			Log.error("[RenderTarget] pSwapChain->GetBuffer() failed");
			return false;
		}

		hr = pDevice->CreateRenderTargetView(pFrameBuffer.Get(), nullptr, pRenderTargetView.GetAddressOf());
		if (FAILED(hr)) {
			Log.error("[RenderTarget] pDevice->CreateRenderTargetView() failed");
			return false;
		}
	}

	return true;
}

void D3DRenderer::Shutdown() {
	Log.info("Shutting down renderer...");
}

/* drawing */
void D3DRenderer::ClearBackground(ColorRGB color) {
	ColorNorm normalized = color.normalized();
	// this only works if ColorNorm is the same layout as what ClearRenderTargetView takes (FLOAT[4])
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), reinterpret_cast<FLOAT*>(&normalized));
}

void D3DRenderer::Present() {
	pSwapChain->Present(1, 0);
}