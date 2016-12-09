#pragma once
#include "constants.h"
#include "block.h"

class GameManager
{
public:
	GameManager();
	bool loadMedia();
	void close();
	bool init();
	void render();

	void drawBackground();

	void addBlock(LTexture* tex, MatrixPoint p);

	bool processColumnInsertion();
	void initWorld();
	void gameOver();
	bool processFalling();
	void initRemovedBlocks();
	void removedBlocksSplash(double dt);
	void searchMathingBlocks(LBlock* block);
	void sweep(LBlock* block, LBlock* other);
	void markFalling(LBlock* block);
	void handleBlockClick(LBlock* block);

	TextureManager textureManager;
	SoundManager soundManager;

	bool isGameOver = false;
	//The window we'll be rendering to
	SDL_Window* gWindow = nullptr;

	//The window renderer
	SDL_Renderer* gRenderer = nullptr;

	// Screens
	StartScreen* startScreen = nullptr;
	GameoverScreen* gameoverScreen = nullptr;

	bool gBlocksSplashing = false;
	/* Time of the start of the timer for the insertion of new columns*/
	int gInsertTimer = 0;

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

	LBlock* gMatrix[MATRIX_WIDTH + 1][MATRIX_HEIGHT];

	// All the blocks of the game
	std::vector<LBlock*> gBlocks;

	// All the blocks that have been matched and are to be removed from blocks and matrix
	std::vector<LBlock*> gRemovedBlocks;
	bool gPiecesAreFalling = false;
	bool gInsertingColumn = false;

	bool gCurrentScreenStartScreen = true;
	bool gCurrentScreenGameover = false;
	bool gCurrentScreenGame = false;

	int gTimerClicks = START_TIMER_CLICKS;
};

