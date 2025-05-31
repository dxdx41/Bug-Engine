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
#include "Renderer/RendererOptions.h"
#include "Timer.h"

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
	RendererOptions opts{};

	Timer mTimer{};

	IGame* pGame{ nullptr };

	void InitializeLogging();
	void CalculateFPS();

	void HandleKey(int key, int action);
	void HandleResize(int width, int height);
	static void GlobalKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GlobalWindowSizeCallback(GLFWwindow* window, int width, int height);
};