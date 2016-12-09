#include <SDL.h> //http://libsdl.org/download-2.0.php
#include <SDL_image.h> //https://www.libsdl.org/projects/SDL_image/
#include <SDL_mixer.h> //https://www.libsdl.org/projects/SDL_mixer/


#include <vector>
#include <random>
#include <iostream>

#include "constants.h"
#include "block.h"
#include "managers.h"
#include "gamemanager.h"

GameManager::GameManager()
{
	startScreen = new StartScreen(this);
	gameoverScreen = new GameoverScreen(this);
}

bool GameManager::loadMedia()
{
	bool success = true;
	if (!soundManager.loadMedia())
		success = false;
	if (!textureManager.loadMedia())
		success = false;
	return success;
}

void GameManager::close()
{
	soundManager.close();

	gBlocks.clear(); // no need to destruct blocks	
	gRemovedBlocks.clear(); // no need to destruct blocks	


	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	// Free screens
	delete startScreen;
	delete gameoverScreen;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool GameManager::init()
{
	//Initialization flag
	bool success = true;
	gCurrentScreenStartScreen = true;
	gCurrentScreenGameover = false;
	gCurrentScreenGame = false;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Ores", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == nullptr)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == nullptr)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
				else
				{
					//Initialize SDL_mixer
					if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
					{
						printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
						success = false;
					}
				}
			}
		}
	}

	return success;
}

/* Render the background composed of other images*/
void GameManager::drawBackground()
{
	// background color
	SDL_SetRenderDrawColor(gRenderer, 244, 173, 66, 0xFF);
	SDL_RenderClear(gRenderer);
	// rectangle under matrix
	auto pp = MatrixToPixelPoint(MatrixPoint(0, 0));
	pp.y += TEXTURE_SIDE + 10;
	SDL_Rect rect = {pp.x, pp.y, MATRIX_WIDTH * TEXTURE_SIDE, 10};
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 125, 0x00, 0x00);
	SDL_RenderFillRect(gRenderer, &rect);
}


void GameManager::render()
{
	drawBackground();

	// render remaining time for insertion bar
	if (!gInsertingColumn)
	{
		double w = (SDL_GetTicks() - gInsertTimer) / double(gTimerClicks);
		if (w >= 1.0)
		{			
			gInsertTimer = SDL_GetTicks();
			if (!isGameOver)
				gInsertingColumn = true;
			w = 1.0;
			// increase difficulty by shortening timer
			gTimerClicks = int(gTimerClicks * TIMER_DIFFICULTY_MULTIPLYER);
		}
		SDL_Rect rect = {TIMER_POS_X, TIMER_POS_Y, int(w * TIMER_WIDTH), TIMER_HEIGHT};
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x22, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &rect);
	}


	//Render blocks
	for (LBlock* blk : gBlocks)
	{
		blk->render();
	}

	//Render splash blocks
	for (auto blk : gRemovedBlocks)
	{
		blk->setAlpha(230);
		blk->render();
	}

	//Update screen
	SDL_RenderPresent(gRenderer);
}

/* Add a block to the matrix and list of blocks with a given texture at coordinage (j,i) */
//void addBlock(LTexture* tex, int j, int i) {
void GameManager::addBlock(LTexture* tex, MatrixPoint p)
{
	LBlock* blk = new LBlock();
	blk->setTexture(tex);
	//blk->setMatrixPos(j, i);
	blk->setMatrixPoint(p);
	//blk->setPixelPos(ColumnMatrixToPixel(j), LineMatrixToPixel(i));
	blk->setPixelPoint(MatrixToPixelPoint(p));
	gMatrix[p.j][p.i] = blk;
	gBlocks.push_back(blk);
}


// After clicking in a block it will check if there are two or more blocks of the same color
void GameManager::handleBlockClick(LBlock* block)
{
	// clean blocks searched flag
	for (auto b : gBlocks)
		b->searched = false;
	searchMathingBlocks(block);
}


/* Mark all the blocks above the given block meaning that they are going to fall due to blocks being removed */
void GameManager::markFalling(LBlock* block)
{
	for (int i = block->getMatrixPoint().i; i < MATRIX_HEIGHT; i++)
	{
		LBlock* el = gMatrix[block->getMatrixPoint().j][i];
		if (el != nullptr)
		{
			el->falling = true;
		}
	}
}

/* Adds the two given blocks to the vector of removed blocks when they share the same texture */
void GameManager::sweep(LBlock* block, LBlock* other)
{
	if (other != nullptr && block->getTexture() == other->getTexture())
	{
		// add to the removed blocks if not contained
		if (std::find(gRemovedBlocks.begin(), gRemovedBlocks.end(), block) == gRemovedBlocks.end())
			gRemovedBlocks.push_back(block);
		markFalling(block);
		// add to the removed blocks if not contained
		if (std::find(gRemovedBlocks.begin(), gRemovedBlocks.end(), other) == gRemovedBlocks.end())
			gRemovedBlocks.push_back(other);
		markFalling(other);
		searchMathingBlocks(other);
		//
		gPiecesAreFalling = true;
	}
}

/* Detect all blocks connected to the first initial block that are of the same color */
void GameManager::searchMathingBlocks(LBlock* block)
{
	if (block->searched) // avoid infinite loops
		return;
	block->searched = true;
	// handle the top block
	if (block->getMatrixPoint().i < MATRIX_HEIGHT - 1)
	{
		LBlock* top = gMatrix[block->getMatrixPoint().j][block->getMatrixPoint().i + 1];
		sweep(block, top);
	}
	// handle the bottom block
	if (block->getMatrixPoint().i > 0)
	{
		LBlock* bot = gMatrix[block->getMatrixPoint().j][block->getMatrixPoint().i - 1];
		sweep(block, bot);
	}
	// handle right block
	if (block->getMatrixPoint().j < MATRIX_WIDTH - 1)
	{
		LBlock* right = gMatrix[block->getMatrixPoint().j + 1][block->getMatrixPoint().i];
		sweep(block, right);
	}
	// handle left block
	if (block->getMatrixPoint().j > 0)
	{
		LBlock* left = gMatrix[block->getMatrixPoint().j - 1][block->getMatrixPoint().i];
		sweep(block, left);
	}
}

void GameManager::removedBlocksSplash(double dt)
{
	if (gRemovedBlocks.size() > 0)
	{
		/*
		* calculate new x
		* calculate new y
		* calculate new angle
		* delete from removedBlocks when block outside screen,
		* call delete on the block to free memory
		*/
		std::vector<LBlock*> outsideBlocks;
		for (auto blk : gRemovedBlocks)
		{
			int x = blk->getPixelPoint().x + int(blk->velocityX * dt);
			int y = blk->getPixelPoint().y + int(blk->velocityY * dt + 10 * dt * dt);

			int a = int(blk->degrees + blk->angularVelocity * dt);
			blk->setDegrees(a);
			blk->setPixelPoint(PixelPoint(x, y));

			if (x < 0)
			{
				outsideBlocks.push_back(blk);
			}
			if (x > SCREEN_WIDTH - 1)
			{
				outsideBlocks.push_back(blk);
			}
			if (y < 0)
			{
				outsideBlocks.push_back(blk);
			}
			if (y > SCREEN_HEIGHT - 1)
			{
				outsideBlocks.push_back(blk);
			}
		}

		for (auto oblk : outsideBlocks)
			gRemovedBlocks.erase(std::remove(gRemovedBlocks.begin(), gRemovedBlocks.end(), oblk), gRemovedBlocks.end());
	}
}

void GameManager::initRemovedBlocks()
{
	// remove matched blocks from the vector of blocks
	for (LBlock* blk : gRemovedBlocks)
	{
		//mTex->setAlpha(255);
		gMatrix[blk->getMatrixPoint().j][blk->getMatrixPoint().i] = nullptr;
		gBlocks.erase(std::remove(gBlocks.begin(), gBlocks.end(), blk), gBlocks.end());
	}


	for (auto blk : gRemovedBlocks)
	{
		/*
		* get random x velocity (-XV,+XV)
		* get random y velocity (-YV,+YV)
		* get random w angular speed (-90,-45) U (45, 90), experimental values
		* gravity "g" constance can be lower to give effect that deleted blocks are light weight
		* angular acceleration is not applicable since there is no force making the blocks rotate faster
		*/
		blk->velocityX = int(fRand(-3, 3));
		blk->velocityY = int(fRand(0, 10));
		blk->angularVelocity = int(fRand(-15, 15));
	}
}

bool GameManager::processFalling()
{
	initRemovedBlocks();

	int blocksFalling = 0;
	// Calculate the new matrix coordinates of each faling block
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		int lowest = 0;
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			if (gMatrix[j][i] != nullptr)
			{
				LBlock* block = gMatrix[j][i];
				gMatrix[j][i] = nullptr;
				gMatrix[j][lowest] = block;
				//block->setMatrixPos(j, lowest);				
				block->setMatrixPoint(MatrixPoint(lowest, j));
				if (lowest != i)
				{
					blocksFalling++;
					//block->falling = true;
				}
				lowest++;
			}
		}
	}


	int blocksSliding = 0;
	int rightest = MATRIX_WIDTH - 1;
	// Calculate the new matrix coordinates of each slidding block to the right
	for (int j = rightest; j >= 0; j--)
	{
		if (gMatrix[j][0] != nullptr)
		{
			if (j != rightest)
			{ // means j has empty collumn to the right				
				for (int i = 0; i < MATRIX_HEIGHT; i++)
				{
					if (gMatrix[j][i] != nullptr)
					{
						LBlock* block = gMatrix[j][i];
						blocksSliding++;
						block->sliding = true;
						gMatrix[j][i] = nullptr;
						gMatrix[rightest][i] = block;
						block->setMatrixPoint(MatrixPoint(i, rightest));
					}
				}
			}
			rightest--;
		}
	}

	int startTime = SDL_GetTicks();
	SDL_Event e;
	bool quit = false;

	while (blocksFalling > 0 && !quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return quit;
			}
		}

		double dt = (SDL_GetTicks() - startTime) / 1000.0;
		//
		removedBlocksSplash(dt);

		// Fall blocks by moving blocks down step by step
		for (LBlock* blk : gBlocks)
		{
			if (blk->falling)
			{
				int x = blk->getPixelPoint().x;
				int y = blk->getPixelPoint().y + int(10 * dt + 5 * dt * dt);
				blk->setPixelPoint(PixelPoint(x, y));
				auto pp = MatrixToPixelPoint(blk->getMatrixPoint());
				int yTarget = pp.y;
				if (blk->getPixelPoint().y >= yTarget)
				{
					blk->falling = false;
					blocksFalling--;
					blk->setPixelPoint(PixelPoint(x, yTarget));
				}
			}
		}
		render();
	}

	while (blocksSliding > 0 && !quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return quit;
			}
		}
		double dt = (SDL_GetTicks() - startTime) / 1000.0;
		//
		removedBlocksSplash(dt);

		// Slide blocks by moving blocks rightway step by step
		for (LBlock* blk : gBlocks)
		{
			if (blk->sliding)
			{
				int x = blk->getPixelPoint().x + int(10 * dt + 5 * dt * dt);
				int y = blk->getPixelPoint().y;
				blk->setPixelPoint(PixelPoint(x, y));
				auto pp = MatrixToPixelPoint(blk->getMatrixPoint());
				int xTarget = pp.x;
				if (blk->getPixelPoint().x >= xTarget)
				{
					blk->sliding = false;
					blocksSliding--;
					blk->setPixelPoint(PixelPoint(xTarget, y));
				}
			}
		}

		render();
	}

	while (gRemovedBlocks.size() > 0)
	{
		double dt = (SDL_GetTicks() - startTime) / 1000.0;
		removedBlocksSplash(dt);
		render();
	}

	return quit;
}


void GameManager::gameOver()
{
	Mix_PlayChannel(-1, soundManager.gGameOverSound, 0);
	gCurrentScreenGame = false;
	gCurrentScreenGameover = true;
	isGameOver = true;
}

void GameManager::initWorld()
{
	isGameOver = false;
	gTimerClicks = START_TIMER_CLICKS;

	gPiecesAreFalling = false;
	gInsertingColumn = false;


	gBlocksSplashing = false;


	for (LBlock* blk : gBlocks)
		delete blk;
	gBlocks.clear();

	for (LBlock* blk : gRemovedBlocks)
		delete blk;
	gRemovedBlocks.clear();

	for (int i = 0; i < MATRIX_HEIGHT; i++)
		for (int j = 0; j < MATRIX_WIDTH + 1; j++)
			gMatrix[i][j] = nullptr;


	gInsertTimer = SDL_GetTicks();
}


bool GameManager::processColumnInsertion()
{
	Mix_PlayChannel(-1, soundManager.gColumnInsertionSound, 0);
	SDL_Event e;
	int startTime = SDL_GetTicks();
	bool finished = false;
	bool quit = false;

	// create random column
	for (int i = 0; i < MATRIX_HEIGHT; i++)
	{
		// plane column just outside the screen to the right
		addBlock(textureManager.randomTexture(), MatrixPoint(i, MATRIX_WIDTH));
	}

	if (gMatrix[0][0] != nullptr)
	{ //if the first column is not NULL then the game is over
		finished = false;
		quit = false;
		isGameOver = true;
		int xTarget = gMatrix[0][0]->getPixelPoint().x - TEXTURE_SIDE;

		while (!finished && !quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
					return quit;
				}
			}

			// make all blocks slide one block to the left and adjust the render and matrix positions
			double dt = (SDL_GetTicks() - startTime) / 1000.0;

			for (LBlock* blk : gBlocks)
			{
				int x = blk->getPixelPoint().x - int(10 * dt);
				int y = blk->getPixelPoint().y;
				blk->setPixelPoint(PixelPoint(x, y));
				int cx = blk->getPixelPoint().x;
				if (cx <= xTarget)
				{
					finished = true; // once one block has shifted then all blocks will be shifted after for loop
					blk->setPixelPoint(PixelPoint(xTarget, y));
				}
			}

			for (int i = 0; i < MATRIX_HEIGHT; i++)
			{
				if (gMatrix[0][i] == nullptr)
					break;
				gRemovedBlocks.push_back(gMatrix[0][i]);
			}

			render();
		}

		startTime = SDL_GetTicks();
		gameOver();
		initRemovedBlocks();
		while (gRemovedBlocks.size() > 0 && !quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					quit = true;
					gameOver();
					return quit;
				}
			}
			double dt = (SDL_GetTicks() - startTime) / 1000.0;
			removedBlocksSplash(dt);
			render();
		}

		return quit;
	}
	// makes all columns shift to the left
	for (int j = 1; j < MATRIX_WIDTH + 1; j++)
	{
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			LBlock* blk = gMatrix[j][i];
			if (blk == nullptr)
				continue;
			gMatrix[j - 1][i] = blk;
			//int newj = blk->getMatrixPoint().j - 1;
			gMatrix[j][i]->setMatrixPoint(MatrixPoint(i, j - 1));
			gMatrix[j][i] = nullptr;
		}
	}


	while (!finished && !quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return quit;
			}
		}

		// make all blocks slide one block to the left and adjust the render and matrix positions
		double dt = (SDL_GetTicks() - startTime) / 1000.0;

		for (LBlock* blk : gBlocks)
		{
			int x = blk->getPixelPoint().x - int(10 * dt + 5 * dt * dt);
			int y = blk->getPixelPoint().y;
			blk->setPixelPoint(PixelPoint(x, y));
			auto pp = MatrixToPixelPoint(blk->getMatrixPoint());
			int xTarget = pp.x;
			if (blk->getPixelPoint().x <= xTarget)
			{
				finished = true; // once one block has shifted then all blocks will be shifted after for loop
				blk->setPixelPoint(PixelPoint(xTarget, y));
			}
		}

		render();
	}

	return quit;
}
