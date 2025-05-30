// 
// Engine
// Responsible for:
// - Windowing and input
// - Rendering / graphics system
// - Timing and main game loop
// - Engine resources (textures, shaders, assets)
// 

#pragma once
#include "IEngine.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <memory>
#include "Renderer/D3DRenderer.h"

class Engine : public IEngine {
public:
	Engine() = default;
	~Engine() override;
	bool Initialize() override;
	void Run(IGame* pGame) override;
	void Shutdown() override;
private:
	/* window */
	GLFWwindow* window{ nullptr };
	HWND hWnd{};
	/* renderer */
	std::unique_ptr<IRenderer> pRenderer{ nullptr };

	IGame* pGame{ nullptr };

	void InitializeLogging();
};