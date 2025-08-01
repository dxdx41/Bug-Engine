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
#include "Util/Math/Vectors.h"
#include "Util/Math/Mat4.h"
#include "PlayerController.h"

class Engine : public IEngine {
public:
	Engine() = default;
	~Engine() override;
	bool Initialize() override;
	void Run() override;
	void Shutdown() override;
private:
	/* window */
	GLFWwindow* window{ nullptr };
	HWND hWnd{};
	/* renderer */
	std::unique_ptr<IRenderer> pRenderer{ nullptr };
	RendererOptions opts{};

	/* game */
	Timer mTimer{};

	std::unique_ptr<PlayerController> pController{ nullptr };

	Vec2 PrevCursor{ 0, 0 };

	Mat4 identity{};
	Vec3 cubePos{ 0, 0, 5 };
	Vec3 cubeRot{ 0, 0, 0 };
	Vec3 cubeScaling{ 1, 1, 1 };

	Vec3 groundPos{ 0, -1, 0 };
	Vec3 groundRot{ 0, 0, 0 };
	Vec3 groundScaling{ 10, 1, 10 };




	void InitializeLogging();
	void CalculateFPS();

	void HandleKey(int key, int action);
	void HandleCursor(double x, double y);
	void HandleResize(int width, int height);
	static void GlobalKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void GlobalCursorCallback(GLFWwindow* window, double xpos, double ypos);
	static void GlobalWindowSizeCallback(GLFWwindow* window, int width, int height);
};