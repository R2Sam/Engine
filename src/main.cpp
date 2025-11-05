#include "Engine/Game.h"

int main ()
{
	Game game(1280, 720, "Template");

	game.Run(60);

	return 0;
}