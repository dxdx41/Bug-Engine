// 
// Renderer
// Abstraction for rendering method
// Exposes renderer agnostic methods
// 

#pragma once
#include "IRenderer.h"
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <wrl.h>

class D3DRenderer : public IRenderer {
public:
	D3DRenderer() = default;
	~D3DRenderer() override;
	bool Initialize(HWND hWnd) override;
	void Shutdown() override;
	
	void ClearBackground(ColorRGB color) override;
	void Present() override;
private:
	HWND hWnd{};
	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView{ nullptr };
};