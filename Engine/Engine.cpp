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
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, Engine::GlobalKeyCallback);
    glfwSetWindowSizeCallback(window, Engine::GlobalWindowSizeCallback);
    Log.info("GLFW window created");
    this->hWnd = glfwGetWin32Window(window);
    if (!hWnd) { Log.error("hWnd is invalid"); }

    /* RENDERER INITIALIZATION */
    pRenderer = std::make_unique<D3DRenderer>();
    if (!pRenderer->Initialize(hWnd, &opts)) {
        Log.error("Renderer initialization failed");
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }


    /* GAME INITIALIZATION */


    return true;
}

void Engine::Run(IGame* pGame) {
    Log.info("Running engine");

    this->pGame = pGame;
    pGame->OnInit();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        pRenderer->BeginFrame();
        
        // optional
        pRenderer->ClearBackground({ 196, 250, 255, 255 });

        pRenderer->EndFrame();
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
    Log.info("----- Logging Started -----");
}


/* object handlers */
void Engine::HandleKey(int key, int action) {
    if (key == GLFW_KEY_F1 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        opts.vSync = !opts.vSync;
        Log.info("vSync: " + std::string((opts.vSync ? "on" : "off")));
    }
}
void Engine::HandleResize(int width, int height) {
    // should renderer handle all resize events or split into different functions like
    // swapchain, rendertargetview, depth/stencil, viewports, scaling
    pRenderer->Resize(width, height);
}

/* global static callbacks */
void Engine::GlobalKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->HandleKey(key, action);
    }
}

// only called once window is "confirmed" to be resized, e.g. when you let go of the window resize bar
void Engine::GlobalWindowSizeCallback(GLFWwindow* window, int width, int height) {
    Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->HandleResize(width, height);
    }
}