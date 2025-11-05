#include "Engine/Game.h"

#include "FirstScene.cpp"

int main ()
{
	Game game(1280, 720, "Template");

	game.SetFirstScene<FirstScene>("First");

	game.Run(60);

	return 0;
}