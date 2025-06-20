
#include <Windows.h>
#include "Engine/Engine.h"
#include "Util/Log.h"
#include "Util/Helper.h"

#include <iostream>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    IEngine* engine{ new Engine() };
    if (!engine->Initialize()) {
        std::cin.get();
        return 1;
    }

    engine->Run();


    engine->Shutdown();
    delete engine;
    return 0;
}