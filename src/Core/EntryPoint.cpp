#pragma once
#include <iostream>
#include "Game.h"

int main()
{
	Game game;

	game.Start();

	while(game.Running())
	{
		game.Update();
	}

	game.End();
	std::cin.get();
	//return 0;
}