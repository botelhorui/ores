//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h> //http://libsdl.org/download-2.0.php
#include <SDL_image.h> //https://www.libsdl.org/projects/SDL_image/
#include <SDL_mixer.h> //https://www.libsdl.org/projects/SDL_mixer/


#include <vector>
#include <random>
#include <algorithm>

#include "constants.h"
#include "block.h"
#include "texture.h"


//Starts up SDL and creates window

bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();

void render();

//The window we'll be rendering to
SDL_Window* gWindow = nullptr;

//The window renderer
SDL_Renderer* gRenderer = nullptr;

//The sound effects that will be used
Mix_Chunk *gClickBlock = nullptr;
Mix_Chunk *gColumnInsertion = nullptr;
Mix_Chunk *gGameOver = nullptr;

bool blocksSplashing = false;

//Scene texture
LTexture gRedSquare;
LTexture gGreySquare;
LTexture gGreenSquare;
LTexture gBlueSquare;
LTexture gYellowSquare;

// All the textures
std::vector<LTexture*> textures;

/* Time of the start of the timer for the insertion of new columns*/
int insertTimer;

/** Matrix of game blocks
 *
 *	The first dimension are the columns of the matrix
 *	The second dimension are the lines of the matrix
 *	
 *	The first column "matrix[0][x]" represents the left-most column 
 *	But the first line "matrix[x][0]" represent the lowest line
 *	
 *	An extra column is used to insert the new column at the right of the matrix
 */
LBlock* matrix[MATRIX_WIDTH + 1][MATRIX_HEIGHT];

// All the blocks of the game
std::vector<LBlock*> blocks;

// All the blocks that have been matched and are to be removed from blocks and matrix
std::vector<LBlock*> removedBlocks;


bool piecesAreFalling = false;
bool insertingColumn = false;





// Converts
PixelPoint MatrixToPixelPoint(MatrixPoint mp)
{
	int y = SCREEN_HEIGHT - (MATRIX_BOT_BORDER + 1 + mp.i) * TEXTURE_SIDE;
	int x = TEXTURE_SIDE * (MATRIX_LEFT_BORDER + mp.j);
	return PixelPoint(x, y);
}

/* Render the background composed of other images*/
void drawBackground()
{
	// background color
	SDL_SetRenderDrawColor(gRenderer, 244, 173, 66, 0xFF);
	SDL_RenderClear(gRenderer);
	// rectangle under matrix
	auto pp = MatrixToPixelPoint(MatrixPoint(0, 0));
	pp.y += TEXTURE_SIDE + 10;
	SDL_Rect rect = { pp.x, pp.y, MATRIX_WIDTH*TEXTURE_SIDE, 10 };
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 125, 0x00, 0x00);
	SDL_RenderFillRect(gRenderer, &rect);
}

/* Generates a random texture from the available ones*/
LTexture* randomTexture()
{
	// http://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, textures.size() - 1);
	int randomTexture = uni(rng);
	return textures[randomTexture];
}

/* Add a block to the matrix and list of blocks with a given texture at coordinage (j,i) */
//void addBlock(LTexture* tex, int j, int i) {
void addBlock(LTexture* tex, MatrixPoint p)
{
	LBlock* blk = new LBlock();
	blk->setTexture(tex);
	//blk->setMatrixPos(j, i);
	blk->setMatrixPoint(p);
	//blk->setPixelPos(ColumnMatrixToPixel(j), LineMatrixToPixel(i));
	blk->setPixelPoint(MatrixToPixelPoint(p));
	matrix[p.j][p.i] = blk;
	blocks.push_back(blk);
}

void searchMathingBlocks(LBlock* block);

// After clicking in a block it will check if there are two or more blocks of the same color
void handleBlockClick(LBlock* block)
{
	// clean blocks searched flag
	for (auto b : blocks)
		b->searched = false;
	searchMathingBlocks(block);
}


/* Mark all the blocks above the given block meaning that they are going to fall due to blocks being removed */
void markFalling(LBlock* block)
{
	for (int i = block->getMatrixPoint().i; i < MATRIX_HEIGHT; i++)
	{
		LBlock* el = matrix[block->getMatrixPoint().j][i];
		if (el != nullptr)
		{
			el->falling = true;
		}
	}
}

/* Adds the two given blocks to the vector of removed blocks when they share the same texture */
void sweep(LBlock* block, LBlock* other)
{
	if (other != nullptr && block->getTexture() == other->getTexture())
	{
		// add to the removed blocks if not contained
		if (std::find(removedBlocks.begin(), removedBlocks.end(), block) == removedBlocks.end())
			removedBlocks.push_back(block);
		markFalling(block);
		// add to the removed blocks if not contained
		if (std::find(removedBlocks.begin(), removedBlocks.end(), other) == removedBlocks.end())
			removedBlocks.push_back(other);
		markFalling(other);
		searchMathingBlocks(other);
		//
		piecesAreFalling = true;
	}
}

/* Detect all blocks connected to the first initial block that are of the same color */
void searchMathingBlocks(LBlock* block)
{
	if (block->searched) // avoid infinite loops
		return;
	block->searched = true;
	// handle the top block
	if (block->getMatrixPoint().i < MATRIX_HEIGHT - 1)
	{
		LBlock* top = matrix[block->getMatrixPoint().j][block->getMatrixPoint().i + 1];
		sweep(block, top);
	}
	// handle the bottom block
	if (block->getMatrixPoint().i > 0)
	{
		LBlock* bot = matrix[block->getMatrixPoint().j][block->getMatrixPoint().i - 1];
		sweep(block, bot);
	}
	// handle right block
	if (block->getMatrixPoint().j < MATRIX_WIDTH - 1)
	{
		LBlock* right = matrix[block->getMatrixPoint().j + 1][block->getMatrixPoint().i];
		sweep(block, right);
	}
	// handle left block
	if (block->getMatrixPoint().j > 0)
	{
		LBlock* left = matrix[block->getMatrixPoint().j - 1][block->getMatrixPoint().i];
		sweep(block, left);
	}
}


bool init()
{
	//Initialization flag
	bool success = true;

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
				}else
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

bool loadMedia()
{
	//Loading success flag
	bool success = true;
	std::string basePath = "../resources/";

	//Load squares
	if (!gRedSquare.loadFromFile(basePath + "red.square.bmp"))
	{
		printf("Failed to load red square texture!\n");
		success = false;
	}
	textures.push_back(&gRedSquare);
	if (!gGreySquare.loadFromFile(basePath + "grey.square.bmp"))
	{
		printf("Failed to load grey square texture!\n");
		success = false;
	}
	textures.push_back(&gGreySquare);
	if (!gGreenSquare.loadFromFile(basePath + "green.square.bmp"))
	{
		printf("Failed to load green square texture!\n");
		success = false;
	}
	textures.push_back(&gGreenSquare);
	if (!gBlueSquare.loadFromFile(basePath + "blue.square.bmp"))
	{
		printf("Failed to load blue square texture!\n");
		success = false;
	}
	textures.push_back(&gBlueSquare);
	if (!gYellowSquare.loadFromFile(basePath + "yellow.square.bmp"))
	{
		printf("Failed to load yellow square texture!\n");
		success = false;
	}
	textures.push_back(&gYellowSquare);

	//Load sound effects
	gClickBlock = Mix_LoadWAV("../resources/metal.click.wav");
	if (gClickBlock == nullptr)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	//Load sound effects
	gColumnInsertion = Mix_LoadWAV("../resources/slidding.wav");
	if (gColumnInsertion == nullptr)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	//Load sound effects
	gGameOver = Mix_LoadWAV("../resources/gameover.wav");
	if (gGameOver == nullptr)
	{
		printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}


	return success;
}

void close()
{
	//Free loaded textures
	for (LTexture* tex : textures)
		tex->free();

	textures.clear();

	for (LBlock* blk : blocks)
		delete blk;
	blocks.clear();

	for (LBlock* blk : removedBlocks)
		delete blk;
	removedBlocks.clear();

	//Free the sound effects
	Mix_FreeChunk(gClickBlock);
	Mix_FreeChunk(gGameOver);
	Mix_FreeChunk(gColumnInsertion);
	gClickBlock = nullptr;
	gGameOver = nullptr;
	gColumnInsertion = nullptr;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;
	gRenderer = nullptr;

	//Quit SDL subsystems
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}


void removedBlocksSplash(double dt)
{
	if(removedBlocks.size() > 0)
	{
		/* TODO
		* calculate new x
		* calculate new y
		* calculate new angle
		* delete from removedBlocks when block outside screen,
		* call delete on the block to free memory
		*/
		std::vector<LBlock*> outsideBlocks;
		for (auto blk : removedBlocks)
		{
			int x = blk->getPixelPoint().x + int(blk->velocityX * dt);
			int y = blk->getPixelPoint().y + int(blk->velocityY * dt + 10 * dt * dt);
			int a = blk->degrees + blk->angularVelocity * dt;
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
			if (y < 0) {
				outsideBlocks.push_back(blk);
			}
			if (y > SCREEN_HEIGHT - 1) {
				outsideBlocks.push_back(blk);
			}
		}

		for (auto oblk : outsideBlocks)
			removedBlocks.erase(std::remove(removedBlocks.begin(), removedBlocks.end(), oblk), removedBlocks.end());
	}
}

void initRemovedBlocks()
{
	// remove matched blocks from the vector of blocks
	for (LBlock* blk : removedBlocks)
	{
		//mTex->setAlpha(255);
		matrix[blk->getMatrixPoint().j][blk->getMatrixPoint().i] = nullptr;
		blocks.erase(std::remove(blocks.begin(), blocks.end(), blk), blocks.end());
	}


	for (auto blk : removedBlocks)
	{
		/*TODO
		* get random x velocity (-XV,+XV)
		* get random y velocity (-YV,+YV)
		* get random w angular speed (-90,-45) U (45, 90), experimental values
		* gravity "g" constance can be lower to give effect that deleted blocks are light weight
		* angular acceleration is not applicable since there is no force making the blocks rotate faster
		*/
		blk->velocityX = fRand(-3, 3);
		blk->velocityY = fRand(0, 10);
		blk->angularVelocity = fRand(-15, 15);
	}
}

bool processFalling()
{
	initRemovedBlocks();

	int blocksFalling = 0;
	// Calculate the new matrix coordinates of each faling block
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		int lowest = 0;
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			if (matrix[j][i] != nullptr)
			{
				LBlock* block = matrix[j][i];
				matrix[j][i] = nullptr;
				matrix[j][lowest] = block;
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
		if (matrix[j][0] != nullptr)
		{
			if (j != rightest)
			{ // means j has empty collumn to the right				
				for (int i = 0; i < MATRIX_HEIGHT; i++)
				{
					if (matrix[j][i] != nullptr)
					{
						LBlock* block = matrix[j][i];
						blocksSliding++;
						block->sliding = true;
						matrix[j][i] = nullptr;
						matrix[rightest][i] = block;						
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
		for (LBlock* blk : blocks)
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
		for (LBlock* blk : blocks)
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
	
	while(removedBlocks.size() > 0)
	{
		double dt = (SDL_GetTicks() - startTime) / 1000.0;
		removedBlocksSplash(dt);
		render();
	}
		
	
	// TODO remove because it becomes empty
	removedBlocks.clear();

	return quit;
}


void gameOver()
{
	Mix_PlayChannel(-1, gGameOver, 0);
	SDL_Delay(6000);
}




void initWorld()
{
	for (int i = 0; i < MATRIX_HEIGHT; i++)
		for (int j = 0; j < MATRIX_WIDTH + 1; j++)
			matrix[i][j] = nullptr;

	
	insertTimer = SDL_GetTicks();
}


void render()
{
	drawBackground();

	// render remaining time for insertion bar
	if (!insertingColumn)
	{
		double w = (SDL_GetTicks() - insertTimer) / double(TIMER_CLICKS);
		if (w >= 1.0)
		{
			insertTimer = SDL_GetTicks();
			insertingColumn = true;
			w = 1.0;
		}
		SDL_Rect rect = {TIMER_POS_X, TIMER_POS_Y, int(w * TIMER_WIDTH), TIMER_HEIGHT};
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x22, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &rect);
	}

	
	//Render blocks
	for (LBlock* blk : blocks)
	{
		blk->render();
	}

	//Render splash blocks
	for (auto blk : removedBlocks)
	{
		blk->setAlpha(230);
		blk->render();
	}

	//Update screen
	SDL_RenderPresent(gRenderer);
}

bool processColumnInsertion()
{
	Mix_PlayChannel(-1, gColumnInsertion, 0);
	SDL_Event e;
	int startTime = SDL_GetTicks();
	bool finished = false;
	bool quit = false;

	// create random column
	for (int i = 0; i < MATRIX_HEIGHT; i++)
	{
		// plane column just outside the screen to the right
		addBlock(randomTexture(), MatrixPoint(i, MATRIX_WIDTH));
	}

	if (matrix[0][0] != nullptr)
	{ //if the first column is not NULL then the game is over
		finished = false;
		quit = false;

		int xTarget = matrix[0][0]->getPixelPoint().x - TEXTURE_SIDE;

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
			
			for (LBlock* blk : blocks)
			{
				int x = blk->getPixelPoint().x - int(10*dt);
				int y = blk->getPixelPoint().y;
				blk->setPixelPoint(PixelPoint(x, y));
				int cx = blk->getPixelPoint().x;
				if (cx <= xTarget)
				{
					finished = true; // once one block has shifted then all blocks will be shifted after for loop
					blk->setPixelPoint(PixelPoint(xTarget, y));
				}
			}

			for(int i=0; i < MATRIX_HEIGHT; i++)
			{
				if (matrix[0][i] == nullptr)
					break;
				removedBlocks.push_back(matrix[0][i]);
			}

			render();
		}
		startTime = SDL_GetTicks();
		initRemovedBlocks();
		while (removedBlocks.size() > 0)
		{
			double dt = (SDL_GetTicks() - startTime) / 1000.0;
			removedBlocksSplash(dt);
			render();
		}


		gameOver();
		
		quit = true;				
		return quit;
	}
	// makes all columns shift to the left
	for (int j = 1; j < MATRIX_WIDTH + 1; j++)
	{
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			LBlock* blk = matrix[j][i];
			if (blk == nullptr)
				continue;
			matrix[j - 1][i] = blk;
			//int newj = blk->getMatrixPoint().j - 1;
			matrix[j][i]->setMatrixPoint(MatrixPoint(i, j - 1));
			matrix[j][i] = nullptr;
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

		for (LBlock* blk : blocks)
		{
			int x = blk->getPixelPoint().x- int(10 * dt + 5 * dt * dt);
			int y = blk->getPixelPoint().y ;
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

/* Game loop  */
void gameLoop()
{
	initWorld();
	generateWorld();

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

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

			// handle mouse highlight and clicks on each block
			for (LBlock* blk : blocks)
			{
				blk->handleEvent(&e);
			}
		}

		

		render();

		if (piecesAreFalling)
		{
			Mix_PlayChannel(-1, gColumnInsertion, 0);
			quit = processFalling();
			piecesAreFalling = false;
		}

		if (insertingColumn)
		{			
			
			quit = processColumnInsertion();		
			insertingColumn = false;
		}
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
			return -1;
		}
		else
		{
			gameLoop();
		}
	}
	
	//Free resources and close SDL
	close();

	return 0;
}
