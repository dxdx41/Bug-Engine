#include "D3DRenderer.h"
#include "Util/Log.h"
#include "Util/Math/Vertices.h"

struct ConstantBuffer
{
	DirectX::XMMATRIX worldViewProj;
};

D3DRenderer::~D3DRenderer()
{
}

bool D3DRenderer::Initialize(HWND hWnd, RendererOptions* pRendererOptions) {
	Log.info("Initializing renderer...");
	this->hWnd = hWnd;
	this->pOpts = pRendererOptions;

	RECT clientRect{};
	GetClientRect(hWnd, &clientRect);
	clientWidth = clientRect.right - clientRect.left;
	clientHeight = clientRect.bottom - clientRect.top;

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
		sd.Width = clientWidth;
		sd.Height = clientHeight;
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
	{
		D3D11_TEXTURE2D_DESC dsDesc{};
		dsDesc.Width = clientWidth;
		dsDesc.Height = clientHeight;
		dsDesc.MipLevels = 1;
		dsDesc.ArraySize = 1;
		dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsDesc.SampleDesc.Count = 1;
		dsDesc.SampleDesc.Quality = 0;
		dsDesc.Usage = D3D11_USAGE_DEFAULT;
		dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dsDesc.CPUAccessFlags = NULL;
		dsDesc.MiscFlags = NULL;

		// returns HRESULT
		pDevice->CreateTexture2D(&dsDesc, nullptr, pDepthStencilBuffer.ReleaseAndGetAddressOf());
		pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), nullptr, pDepthStencilView.ReleaseAndGetAddressOf());

		pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
	}


	/* setup viewport */
	{	// set it to the full client area
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.Width = static_cast<FLOAT>(clientWidth);
		vp.Height = static_cast<FLOAT>(clientHeight);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		pContext->RSSetViewports(1, &vp);
	}



	/* shader compilation */
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



	/* setup input layout */
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT hr = pDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), pInputLayout.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		Log.error("failed to create input layout");
		return false;
	}

	pContext->IASetInputLayout(pInputLayout.Get());

	/* create static resources */

	// pos3 color4
	BasicVertex vertexData[] = {
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Red
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Green
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Blue
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Yellow

		// Back face (z = +0.5)
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } }, // Magenta
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // Cyan
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // White
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Black
	};

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = sizeof(vertexData);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData = { vertexData };

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, pCubeVertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		Log.error("Failed to create cube vertex buffer");
		return false;
	}

	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, pCubeVertexBuffer.GetAddressOf(), &stride, &offset);

	UINT indices[36] = {
		0, 2, 1, // front
		0, 3, 2,

		5, 7, 4, // back
		5, 6, 7,

		3, 6, 2, // top
		3, 7, 6,
		1, 4, 0, // bottom
		1, 5, 4,
		1, 6, 5, // right
		1, 2, 6,
		4, 3, 0, // left
		4, 7, 3
	};

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData = { indices };

	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexSubresourceData, pCubeIndexBuffer.ReleaseAndGetAddressOf());
	if (FAILED(hr)) {
		Log.error("Failed to create cube index buffer");
		return false;
	}

	pContext->IASetIndexBuffer(pCubeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	/* setup rasterizer and depth/stencil states */

	// normal rasterizer state
	D3D11_RASTERIZER_DESC1 nrm{};
	nrm.FillMode = D3D11_FILL_SOLID;
	nrm.CullMode = D3D11_CULL_BACK;
	nrm.FrontCounterClockwise = false;
	nrm.DepthClipEnable = true;
	pDevice->CreateRasterizerState1(&nrm, pNormalRSState.ReleaseAndGetAddressOf());

	D3D11_RASTERIZER_DESC1 wire{};
	wire.FillMode = D3D11_FILL_WIREFRAME;
	wire.CullMode = D3D11_CULL_BACK; // maybe needs D3D11_CULL_NONE
	wire.FrontCounterClockwise = false;
	wire.DepthClipEnable = true;
	pDevice->CreateRasterizerState1(&wire, pWireframeRSState.ReleaseAndGetAddressOf());

	/* setup to draw */

	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	pDevice->CreateBuffer(&cbd, nullptr, &pConstantBuffer);

	pContext->RSSetState(pNormalRSState.Get());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void D3DRenderer::Shutdown() {
	// ??????
}

void D3DRenderer::OnResize(int width, int height) {
	clientWidth = width;
	clientHeight = height;
	// This needs to handle everything on resize, not just the swapchain/backbuffer

	pRenderTargetView.Reset(); // let go of backbuffer
	pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, NULL);
	
	{	// create new render target view based on resized swapchain
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer{ nullptr };
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.ReleaseAndGetAddressOf()));
		pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pRenderTargetView.ReleaseAndGetAddressOf());
	}
	// set viewports
	D3D11_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<FLOAT>(clientWidth);
	vp.Height = static_cast<FLOAT>(clientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pContext->RSSetViewports(1, &vp);

}

float D3DRenderer::AspectRatio() const {
	return static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
}

/* drawing */

void D3DRenderer::BeginFrame() {
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	pContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), nullptr);
}


void D3DRenderer::EndFrame() {
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

void D3DRenderer::DrawCube(PlayerController* pController, Vec3 pos, Vec3 rotation, Vec3 scaling) {
	using namespace DirectX;

	XMMATRIX S = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
	XMMATRIX SRT = S * R * T;
	XMStoreFloat4x4(&mWorld, SRT);

	XMMATRIX projec = XMMatrixPerspectiveFovLH(XM_PIDIV4, AspectRatio(), 0.1f, 1000.0f);
	XMStoreFloat4x4(&mProj, projec);

	XMVECTOR position = XMVectorSet(pController->m_Pos.x, pController->m_Pos.y, pController->m_Pos.z, 1.0f);

	XMVECTOR target = XMVectorAdd(position, pController->GetView());
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&mView, V);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	ConstantBuffer cb{};
	cb.worldViewProj = XMMatrixTranspose(worldViewProj); // Transpose before sending

	pContext->IASetInputLayout(pInputLayout.Get());
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(BasicVertex);
	UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, pCubeVertexBuffer.GetAddressOf(), &stride, &offset);
	pContext->IASetIndexBuffer(pCubeIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	pContext->UpdateSubresource(pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	pContext->VSSetConstantBuffers(0, 1, pConstantBuffer.GetAddressOf());


	pContext->DrawIndexed(36, 0, 0);
}

/* private functions */

bool D3DRenderer::CompileShaders() {
	// REDO THIS LATER



	{

	}

	return true;
}