// 
// Game Interface
// 

#pragma once

class IGame {
public:
	virtual ~IGame() = default;
	virtual void OnInit() = 0;
};