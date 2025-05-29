#pragma once
#include <Windows.h>
#include <stdio.h>

inline void DebugConsole() {
	AllocConsole();
	SetConsoleTitle(TEXT("Debug Console"));
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);
}