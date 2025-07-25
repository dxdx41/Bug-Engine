// 
// Renderer
// Abstraction for rendering method
// Exposes renderer agnostic methods
// 

#pragma once
#include "IRenderer.h"
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include <wrl.h>
#include "Engine/PlayerController.h"

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

	void DrawCube(PlayerController* pController, Vec3 pos, Vec3 rotation, Vec3 scaling) override;
private:
	HWND hWnd{};
	UINT clientWidth{}, clientHeight{};
	RendererOptions* pOpts{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Device1> pDevice{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> pContext{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11RasterizerState1> pNormalRSState{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11RasterizerState1> pWireframeRSState{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Buffer> pCubeVertexBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCubeIndexBuffer{ nullptr };

	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer{ nullptr };

	DirectX::XMMATRIX gWorldViewProj{};
	DirectX::XMFLOAT4X4 mWorld{};
	DirectX::XMFLOAT4X4 mView{};
	DirectX::XMFLOAT4X4 mProj{};
};