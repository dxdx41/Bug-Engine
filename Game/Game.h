// 
// Game
// Responsible for:
// - game specific logic
// - game objects and entities
// - rending ui and hud
// - scene and level management
// - loading gameplay assets
// 

#pragma once
#include "IGame.h"

class Game : public IGame {
public:
	Game() = default;
	~Game();
	
	void OnInit() override;
private:

	bool vSync{ false }; // not used right now

};