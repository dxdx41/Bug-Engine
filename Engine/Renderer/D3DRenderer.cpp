#include "D3DRenderer.h"
#include "Util/Log.h"

D3DRenderer::~D3DRenderer()
{
}

bool D3DRenderer::Initialize(HWND hWnd, RendererOptions* pRendererOptions) {
	Log.info("Initializing renderer...");
	this->hWnd = hWnd;
	this->pOpts = pRendererOptions;

	/* create device and device context */
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
			Log.info("Graphics Device: " + std::string(desc_w.begin(), desc_w.end()));

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
		if (FAILED(hr)) {
			Log.error("Failed to create swapchain");
			return false;
		}
	}

	/* create render target view */
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

	/* create depth/stencil buffer */



	/* setup viewport */

	RECT clientRect{};
	GetClientRect(hWnd, &clientRect);
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(clientRect.right - clientRect.left), (FLOAT)(clientRect.bottom - clientRect.top), 0.0f, 1.0f };
	pContext->RSSetViewports(1, &viewport);

	/* shader compilation */
	if (!CompileShaders()) return false;


	/* setup input layout */



	/* create static resources */



	/* setup rasterizer and depth/stencil states */



	// EVERYTHING FROM HERE DOWN IS TEMPORARY



	UINT numVerts;
	UINT stride;
	UINT offset;
	{
		float vertexData[] = { // x, y, r, g, b, a
			0.0f,  0.5f, 0.f, 1.f, 0.f, 1.f,
			0.5f, -0.5f, 1.f, 0.f, 0.f, 1.f,
			-0.5f, -0.5f, 0.f, 0.f, 1.f, 1.f
		};
		stride = 6 * sizeof(float);
		numVerts = sizeof(vertexData) / stride;
		offset = 0;

		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(vertexData);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

		HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, pVertexBuffer.GetAddressOf());
		if (FAILED(hr)) {
			Log.error("Failed to create buffer");
			return false;
		}
	}

	/* setup to draw */
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);

	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->IASetInputLayout(pInputLayout.Get());

	pContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);

	return true;
}

void D3DRenderer::Shutdown() {
}

void D3DRenderer::Resize(int width, int height) {
	pRenderTargetView.Reset(); // let go of backbuffer
	pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);
	
	{	// create new render target view based on resized swapchain
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer{ nullptr };
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.ReleaseAndGetAddressOf()));
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView.ReleaseAndGetAddressOf());
	}
	// set viewports
	RECT clientRect{};
	GetClientRect(hWnd, &clientRect);
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(clientRect.right - clientRect.left), (FLOAT)(clientRect.bottom - clientRect.top), 0.0f, 1.0f };
	pContext->RSSetViewports(1, &viewport);

}

/* drawing */

void D3DRenderer::BeginFrame() {
	static FLOAT clr[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), clr);
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
}


void D3DRenderer::EndFrame() {
	pContext->Draw(3, 0); // shouldn't be in here, hardcoded for triangle
	pSwapChain->Present(pOpts->vSync ? 1 : 0, 0);
}


void D3DRenderer::ClearBackground(ColorRGB color) {
	ColorNorm normalized = color.normalized();
	// this only works if ColorNorm is the same layout as what ClearRenderTargetView takes (FLOAT[4])
	pContext->ClearRenderTargetView(pRenderTargetView.Get(), reinterpret_cast<FLOAT*>(&normalized));
}

void D3DRenderer::DrawRect(Rect rect, ColorRGB color) {

}
void D3DRenderer::DrawFilledRect(Rect rect, ColorRGB color, float thickness) {

}
void D3DRenderer::DrawLine(Vec2 pos, ColorRGB color, float thickness) {

}


/* private functions */

bool D3DRenderer::CompileShaders() {
	// REDO THIS LATER

	/* vertex shader */
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob{ nullptr };
	{
		HRESULT hr = D3DCompileFromFile(TEXT("assets\\shaders\\VertexShader.hlsl"), nullptr, nullptr, "vs_main", "vs_5_0", 0, 0, vsBlob.ReleaseAndGetAddressOf(), nullptr);
		if (FAILED(hr)) {
			Log.error("failed to compile shaders from file");
			return false;
		}

		hr = pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, pVertexShader.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			Log.error("failed to create vertex shader");
			return false;
		}
	}
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0);

	/* pixel shader */
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob{ nullptr };
	{	
		HRESULT hr = D3DCompileFromFile(TEXT("assets\\shaders\\PixelShader.hlsl"), nullptr, nullptr, "ps_main", "ps_5_0", 0, 0, psBlob.ReleaseAndGetAddressOf(), nullptr);
		if (FAILED(hr)) {
			Log.error("failed to compile shaders from file");
			return false;
		}

		hr = pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, pPixelShader.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			Log.error("failed to create vertex shader");
			return false;
		}
	}
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0);

	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		HRESULT hr = pDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), pInputLayout.ReleaseAndGetAddressOf());
		if (FAILED(hr)) {
			Log.error("failed to create input layout");
			return false;
		}
	}

	return true;
}