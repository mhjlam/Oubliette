#include "constants.h"
#include "game.h"

int main(int argc, char* argv[])
{
	// Welcome message
	std::cout << "Welcome to the Oubliette" << std::endl;
	std::cout << "Your quest is to escape from the underground." << std::endl;

	// Create and initialize the game
	Game::init(argc,argv);
	Game::play();

	return 0;
}

