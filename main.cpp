//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h> //http://libsdl.org/download-2.0.php
#include <SDL_image.h> //https://www.libsdl.org/projects/SDL_image/
#include <SDL_mixer.h> //https://www.libsdl.org/projects/SDL_mixer/


#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "block.h"
#include "texture.h"
#include "managers.h"
#include "gamemanager.h"

GameManager gameManager;

/* Game loop  */
bool gameLoop()
{
	gameManager.initWorld();
	generateWorld();

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//While application is running
	while (!quit && !gameManager.isGameOver)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			// handle mouse highlight and clicks on each block
			for (LBlock* blk : gameManager.gBlocks)
			{
				blk->handleEvent(&e);
			}
		}


		gameManager.render();

		if (gameManager.gPiecesAreFalling)
		{
			Mix_PlayChannel(-1, gameManager.soundManager.gClickBlockSound, 0);
			quit = gameManager.processFalling();
			gameManager.gPiecesAreFalling = false;
		}

		if (gameManager.gInsertingColumn)
		{
			quit = gameManager.processColumnInsertion();
			gameManager.gInsertingColumn = false;
		}
	}

	return quit;
}


int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!gameManager.init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!gameManager.loadMedia())
		{
			printf("Failed to load media!\n");
			return -1;
		}
		else
		{
			SDL_Event e;
			bool quit = false;
			
			//While application is running
			while (!quit)
			{
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					if (gameManager.gCurrentScreenStartScreen)
					{
						gameManager.startScreen->handleEvent(&e);
					}
					else //game over
					{
						gameManager.gameoverScreen->handleEvent(&e);
					}
				}
				if (quit)
					break;

				if (gameManager.gCurrentScreenStartScreen)
				{
					gameManager.startScreen->render();
				}
				else if (gameManager.gCurrentScreenGame)
				{
					quit = gameLoop();
				}
				else if (gameManager.gCurrentScreenGameover)
				{
					gameManager.gameoverScreen->render();
				}
				else // default case, hope it doesnt happen
				{
					std::cout << "Invalid game state, defaulting to start screen";
					gameManager.startScreen->render();
				}
			}
		}
	}

	//Free resources and close SDL
	gameManager.close();

	return 0;
}
