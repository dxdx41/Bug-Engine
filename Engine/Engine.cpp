#include "Engine.h"
#include "Util/Log.h"
#include <sstream>

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

    Log.info("Aspect ratio: " + std::to_string(pRenderer->AspectRatio()));


    /* GAME INITIALIZATION */


    return true;
}

void Engine::Run() {
    Log.info("Running engine");

    mTimer.Reset();
    while (!glfwWindowShouldClose(window)) {
        mTimer.Tick();
        CalculateFPS();
        glfwPollEvents();

        constexpr float speed = 0.05f;
        if (glfwGetKey(window, GLFW_KEY_W)) {
            camera.z += speed;
        }
        if (glfwGetKey(window, GLFW_KEY_S)) {
            camera.z -= speed;
        }
        if (glfwGetKey(window, GLFW_KEY_A)) {
            camera.x -= speed;
        }
        if (glfwGetKey(window, GLFW_KEY_D)) {
            camera.x += speed;
        }

        if (glfwGetKey(window, GLFW_KEY_UP)) {
            cubeRot.x += speed;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN)) {
            cubeRot.x -= speed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT)) {
            cubeRot.y += speed;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
            cubeRot.y -= speed;
        }

        pRenderer->BeginFrame();
        
        // optional
        pRenderer->ClearBackground({ 0, 0, 0, 255 });
        pRenderer->DrawCube(camera, cubePos, cubeRot);

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
#ifdef _DEBUG
    AllocConsole();
    SetConsoleTitle(TEXT("Debug Console"));
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
#endif
    Logger::Init();
    Log.info("----- Logging Started -----");
}

void Engine::CalculateFPS() {
    static int frameCount{ 0 };
    static float timeElapsed{ 0.0f };

    ++frameCount;

    constexpr float interval{ 2.0f };
    float currentTime = mTimer.TotalTime();
    if ((currentTime - timeElapsed) >= interval) {
        float fps = static_cast<float>(frameCount) / interval;

        std::wostringstream oss{};
        oss.precision(6);
        oss << TEXT("FPS: ") << fps;
        SetWindowText(hWnd, oss.str().c_str());
        frameCount = 0;
        timeElapsed = currentTime;
    }
}

/* object handlers */
void Engine::HandleKey(int key, int action) {
    if (key == GLFW_KEY_F1 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        opts.vSync = !opts.vSync;
        Log.info("vSync: " + std::string((opts.vSync ? "on" : "off")));
    }
}
void Engine::HandleResize(int width, int height) {
    pRenderer->OnResize(width, height);
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