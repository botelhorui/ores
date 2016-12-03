//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include "texture.h"
#include "block.h"
#include "common.h"


//Starts up SDL and creates window
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();
void handleBlockClick(LBlock* block);
void render();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

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

// Matrix of game blocks
// it has an extra column to be used to insert the right column
LBlock* matrix[MATRIX_WIDTH + 1][MATRIX_HEIGHT];

// All the blocks of the game
std::vector<LBlock*> blocks;

// All the blocks that have been matched and are to be removed from blocks and matrix
std::vector<LBlock*> removedBlocks;


bool piecesAreFalling = false;
bool insertingColumn = false;

/* Convert a matrix coordinate to a screen pixel XY coordinate*/
int matrixToPixelHorizontal(int j) {
	return TEXTURE_SIDE * (MATRIX_LEFT_BORDER + j);
}

/* Convert a matrix coordinate to a screen pixel XY coordinate*/
int matrixToPixelVertical(int i) {
	return SCREEN_HEIGHT - (MATRIX_BOT_BORDER + 1 + i) * TEXTURE_SIDE;
}

/* Render the background composed of other images*/
void drawBackground() {
	SDL_SetRenderDrawColor(gRenderer, 244, 173, 66, 0xFF);
	SDL_RenderClear(gRenderer);
}

/* Generates a random texture from the available ones*/
LTexture* randomTexture() {
	// http://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, textures.size() - 1);
	int randomTexture = uni(rng);
	return textures[randomTexture];
}

/* Add a block to the matrix and list of blocks with a given texture at coordinage (j,i) */
void addBlock(LTexture* tex, int j, int i) {
	LBlock* blk = new LBlock();
	blk->setTexture(tex);
	blk->setMatrixPosition(j, i);
	blk->setRenderPosition(matrixToPixelHorizontal(j), matrixToPixelVertical(i));
	matrix[j][i] = blk;
	blocks.push_back(blk);
}

void searchMathingBlocks(LBlock* block);

void handleBlockClick(LBlock* block) {
	// clean blocks search flag
	for (auto b : blocks)
		b->searched = false;
	searchMathingBlocks(block);		
}


/* Mark all the blocks that will be falling due to blocks being removed */
void markFalling(LBlock* block) {
	for (int i = block->matrixPos.y; i < MATRIX_HEIGHT; i++) {
		LBlock* el = matrix[block->matrixPos.x][i];
		if (el != NULL) {
			el->falling = true;
		}
	}	
}

/* Adds to the removed blocks all blocks that match texture*/
void sweep(LBlock* block, LBlock* other) {
	if (other != NULL && block->getTexture() == other->getTexture()) {
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
void searchMathingBlocks(LBlock* block) {
	if (block->searched) // avoid infinite loops
		return;
	block->searched = true;
	// handle the top block
	if (block->matrixPos.y < MATRIX_HEIGHT - 1) {
		LBlock* top = matrix[block->matrixPos.x][block->matrixPos.y + 1];
		sweep(block, top);		
	}
	// handle the bottom block
	if (block->matrixPos.y > 0) {
		LBlock* bot = matrix[block->matrixPos.x][block->matrixPos.y - 1];
		sweep(block, bot);
	}
	// handle right block
	if (block->matrixPos.x < MATRIX_WIDTH - 1) {
		LBlock* right = matrix[block->matrixPos.x+1][block->matrixPos.y];
		sweep(block, right);
	}
	// handle left block
	if (block->matrixPos.x > 0) {
		LBlock* left = matrix[block->matrixPos.x-1][block->matrixPos.y];
		sweep(block, left);
	}
}



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
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
	return success;
}

void close()
{
	//Free loaded textures
	for (LTexture* tex : textures)
		tex->free();
	textures.clear();

	for (LBlock* blk : blocks) 
		free(blk);
	
	blocks.clear();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool processFalling() {	
	int blocksFalling = 0;

	// Calculate the new matrix coordinates of each faling block
	for (int j = 0; j < MATRIX_WIDTH; j++) {
		int lowest = 0;
		for (int i = 0; i < MATRIX_HEIGHT; i++) {
			if (matrix[j][i] != NULL) {
				LBlock* block = matrix[j][i];				
				matrix[j][i] = NULL;
				matrix[j][lowest] = block;
				block->setMatrixPosition(j, lowest);				
				
				if (lowest != i) {
					blocksFalling++;
				}
				lowest++;
			}			
		}
	}
	
	int startTime = SDL_GetTicks();
	SDL_Event e;
	bool quit = false;

	while (blocksFalling > 0 && !quit ) {
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

		for (LBlock* it : blocks) {
			if (it->falling) {		
				it->pixelPos.y = it->pixelPos.y + (int)(10*dt + 5*dt*dt);	
				int yTarget = matrixToPixelVertical(it->matrixPos.y);
				if (it->pixelPos.y >= yTarget) {
					it->falling = false;
					blocksFalling--;
					it->pixelPos.y = yTarget;
				}
			}
		}
		render();

	}
	int blocksSliding = 0;
	int highest = MATRIX_WIDTH-1;
	for (int j = highest; j > 0; j--) {
		if (matrix[j][0] != NULL) {
			if (j != highest) { // means j has empty collumn to the right				
				for (int i = 0; i < MATRIX_HEIGHT; i++) {
					if (matrix[j][i] != NULL) {
						LBlock* block = matrix[j][i];
						blocksSliding++;
						block->falling = true;
						matrix[j][i] = NULL;
						matrix[highest][i] = block;
						block->setMatrixPosition(highest, i);
					}
				}				
			}
			highest--;
		}
			
	}

	while (blocksSliding > 0 && !quit) {
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

		for (LBlock* it : blocks) {
			if (it->falling) {
				it->pixelPos.x = it->pixelPos.x + (int)(10 * dt + 5 * dt*dt);
				int xTarget = matrixToPixelHorizontal(it->matrixPos.x);
				if (it->pixelPos.x >= xTarget) {
					it->falling = false;
					blocksSliding--;
					it->pixelPos.x = xTarget;
				}
			}
		}

		render();
	}

	return quit;
}



void gameOver() {

}

bool processColumnInsertion() {
	SDL_Event e;

	bool quit = false;
	// create random column
	int j = MATRIX_WIDTH; //last column
	for (int i = 0; i < MATRIX_HEIGHT; i++) {
		// plane column just outside the screen to the right
		addBlock(randomTexture(), j, i);
	}		
	
	if (matrix[0][0] != NULL) { //if the first column is not NULL then the game is over
		gameOver();
		quit = true;
		return quit;
	}
	// make column slide to the left until next to the last column
	for (int j = 1; j < MATRIX_WIDTH + 1; j++) {
		for (int i = 0; i < MATRIX_HEIGHT; i++) {
			LBlock* blk = matrix[j][i];
			if (blk == NULL)
				continue;
			matrix[j - 1][i] = matrix[j][i];
			matrix[j][i]->matrixPos.x--;
			matrix[j][i] = NULL;
		}
	}
	int startTime = SDL_GetTicks();
	bool finished = false;

	while (!finished && !quit) {
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
		
		for (LBlock* it : blocks) {			
			it->pixelPos.x = it->pixelPos.x - (int)(5 * dt + 5 * dt*dt);
		
			int xTarget = matrixToPixelHorizontal(it->matrixPos.x);
			if (it->pixelPos.x <= xTarget) {
				finished = true; // when one block is aligned then all other are too
				it->pixelPos.x = xTarget;
			}
		}

		render();

	}

	return quit;
}

/* Used for tests */
void generateWorld2() {
	int j = GAME_FIRST_COLUMN;

	addBlock(&gGreenSquare, j, 1);
	addBlock(&gGreenSquare, j, 2);
	addBlock(&gGreySquare, j, 3);
	addBlock(&gRedSquare, j++, 0);
	for (; j < MATRIX_WIDTH; j++)
		addBlock(&gBlueSquare, j, 0);
}


void generateWorld() {	
	for (int j = GAME_FIRST_COLUMN; j < MATRIX_WIDTH; j++) {
		for (int i = 0; i < MATRIX_HEIGHT; i++) {
			addBlock(randomTexture(), j, i);
		}
	}
}

void initWorld() {
	for (int i = 0; i < MATRIX_HEIGHT; i++)
		for (int j = 0; j < MATRIX_WIDTH+1; j++)
			matrix[i][j] = NULL;

	insertTimer = SDL_GetTicks();
}




void render() {
	drawBackground();

	// render remaining time bar
	if (!insertingColumn) {
		double w = (SDL_GetTicks() - insertTimer) / (double)TIMER_CLICKS;
		if (w >= 1.0) {
			insertTimer = SDL_GetTicks();
			insertingColumn = true;
			w = 1.0;
		}
		SDL_Rect rect = { TIMER_POS_X, TIMER_POS_Y, (int)(w * TIMER_WIDTH), TIMER_HEIGHT };
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x22, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &rect);
	}

	//Render blocks
	for (LBlock* it : blocks) {
		it->render();
	}

	//Update screen
	SDL_RenderPresent(gRenderer);
}

/* Game loop  */
void gameLoop() {
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
			for (LBlock* it : blocks) {				
				it->handleEvent(&e);
			}
		}

		// remove matched blocks from the vector of blocks
		for (LBlock* it : removedBlocks) {
			matrix[it->matrixPos.x][it->matrixPos.y] = NULL;
			blocks.erase(std::remove(blocks.begin(), blocks.end(), it), blocks.end());
		}
		// TODO ..
		removedBlocks.clear();
		
		render();		

		if (piecesAreFalling) {
			quit = processFalling();
			piecesAreFalling = false;
		}

		if (insertingColumn) {
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