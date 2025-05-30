// 
// Renderer Interface
// 

#pragma once
#include <Windows.h>
#include "Util/Color.h"
#include "Util/Types.h"

class IRenderer {
public:
	/* general */
	virtual ~IRenderer() = default;
	virtual bool Initialize(HWND hWnd) = 0;
	virtual bool CompileShaders() = 0;
	virtual void Shutdown() = 0;

	/* drawing */
	virtual void ClearBackground(ColorRGB color) = 0;
	virtual void DrawRect(Rect rect, ColorRGB color) = 0;
	virtual void DrawFilledRect(Rect rect, ColorRGB color, float thickness) = 0;
	virtual void DrawLine(Vec2 pos, ColorRGB color, float thickness) = 0;

	virtual void Present() = 0;
};