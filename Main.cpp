
#include <Windows.h>
#include "Engine/Engine.h"
#include "Game/Game.h"
#include "Util/Log.h"
#include "Util/Helper.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    IEngine* engine{ new Engine() };
    engine->Initialize();
    Game game;

    engine->Run();

    engine->Shutdown();
    return 0;
}