#include "Engine.h"
#include "Util/Log.h"

Engine::~Engine()
{
}

bool Engine::Initialize() {
    InitializeLogging(); // do not log before this
    Log.info("Starting engine...");

    /* GLFW AND WINDOW CREATION */
    if (!glfwInit())
        return false;
    Log.info("GLFW initialized");

    window = glfwCreateWindow(1280, 720, "the game", nullptr, nullptr);
    if (!window)
    {
        Log.error("Failed to create window, terminating GLFW");
        glfwTerminate();
        return false;
    }
    Log.info("GLFW window created");
    hWnd = glfwGetWin32Window(window);
    if (!hWnd) { Log.error("hWnd is invalid"); }

    /* RENDERER INITIALIZATION */
    pRenderer = std::make_unique<D3DRenderer>();
    pRenderer->Initialize(hWnd);


    /* GAME INITIALIZATION */


    return true;
}

void Engine::Run() {
    Log.info("Running engine");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        pRenderer->ClearBackground({ 0, 0, 0, 255 });
        pRenderer->Present();
    }
}

void Engine::Shutdown() {
    Log.info("Shutting down engine...");

    Log.info("Shutting down renderer...");
    pRenderer->Shutdown();

    /* GLFW AND WINDOW DESTRUCTION */
    Log.info("Destroying window and GLFW");
    glfwDestroyWindow(window);
    glfwTerminate();
}

/* Private Functions */
void Engine::InitializeLogging() {
    AllocConsole();
    SetConsoleTitle(TEXT("Debug Console"));
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
    Logger::Init();
    Log.info("<< Logging >>");
}