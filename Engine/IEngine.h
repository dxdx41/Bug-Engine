// 
// Engine Interface
// 

#pragma once
#include "Game/IGame.h"

class IEngine {
public:
	// engine methods
	virtual ~IEngine() = default;
	virtual bool Initialize() = 0;
	virtual void Run(IGame* pGame) = 0;
	virtual void Shutdown() = 0;
};