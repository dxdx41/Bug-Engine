// 
// Engine Interface
// 

#pragma once

class IEngine {
public:
	// engine methods
	virtual ~IEngine() = default;
	virtual bool Initialize() = 0;
	virtual void Run() = 0;
	virtual void Shutdown() = 0;
};