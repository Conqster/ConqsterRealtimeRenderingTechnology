#pragma once
#include <iostream>
#include "Game.h"

int main()
{
	Game* new_game = new Game();

	new_game->OnStart();
	new_game->Run();

	std::cin.get();
	//return 0;
}