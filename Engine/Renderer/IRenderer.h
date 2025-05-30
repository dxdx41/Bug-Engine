// 
// Renderer Interface
// 

#pragma once
#include <Windows.h>
#include "Util/Color.h"

class IRenderer {
public:
	/* general */
	virtual ~IRenderer() = default;
	virtual bool Initialize(HWND hWnd) = 0;
	virtual void Shutdown() = 0;

	/* drawing */
	virtual void ClearBackground(ColorRGB color) = 0;

	virtual void Present() = 0;
};