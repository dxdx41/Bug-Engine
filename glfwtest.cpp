#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include "Util/Log.h"
#include "Util/Helper.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // call this stuff in the engine not here
    DebugConsole(true);
    Logger::Init();
    Log.info("info");
    Log.warning("warning");
    Log.error("ERROR");

    GLFWwindow* window;

    // initialize glfw
    if (!glfwInit())
        return -1;

    // create window and get handle to window
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    HWND hWnd = glfwGetWin32Window(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}