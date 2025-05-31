// 
// Renderer
// Abstraction for rendering method
// Exposes renderer agnostic methods
// 

#pragma once
#include "IRenderer.h"
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <wrl.h>

class D3DRenderer : public IRenderer {
public:
	D3DRenderer() = default;
	~D3DRenderer() override;

	bool Initialize(HWND hWnd, RendererOptions* pRendererOptions) override;
	bool CompileShaders() override;
	void Shutdown() override;
	void OnResize(int width, int height) override;

	float AspectRatio() const override;
	
	void BeginFrame() override;
	void EndFrame() override;

	void ClearBackground(ColorRGB color) override;
	void DrawRect(Rect rect, ColorRGB color) override;
	void DrawFilledRect(Rect rect, ColorRGB color, float thickness) override;
	void DrawLine(Vec2 pos, ColorRGB color, float thickness) override;
private:
	HWND hWnd{};
	UINT clientWidth{}, clientHeight{};
	RendererOptions* pOpts{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer{ nullptr };
};