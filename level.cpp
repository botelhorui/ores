#include "constants.h"
#include "geral.h"
#include "gamemanager.h"
/*************************************************************************************
 *************************************************************************************
 * "generateWorld" is the function called to populate the matrix with a configuration of blocks at the initialization.
 * 
 * For testing the execution of the game, multiple functions are used, each with different levels that test the different boundaries of the game
 * 
 * Instructions: in each function signature append random character after "generateWorld" except on the function with intended level to test
 *************************************************************************************
 *************************************************************************************
 */


/*
 * #######
 */
void generateWorldfgfg()
{
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, j));
	}
}

/**
 * O##
 */
void generateWorldadas()
{
	gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
}

/**
* O########
*/

void generateWorldasf()
{
	gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, 0));
	for (int j = 1; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, j));
	}
}

/**
 * O
 * O
 * O
 * O
 * O###########
 */
void generateWorldasd()
{
	for (int i = 0; i < MATRIX_HEIGHT; i++)
	{
		gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(i, 0));
	}
	for (int j = 1; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, j));
	}
}

/** OOOOOOOOOO
*  OOOOOOOOOO
*  OOOOOOOOOO
*  OOOOOOOOOO
*  OOOOOOOOOO##
*/
void generateWorldfghfgh()
{
	for (int j = 0; j < MATRIX_WIDTH - 2; j++)
	{
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			if (j == MATRIX_WIDTH - 3 && i == 0)
				i = 1;
			gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(i, j));
		}
	}
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
}

/*
 * X
 * O
 * O
 * O
 * O
 * O
 */
void generateWorldccc()
{
	for (int i = 0; i < MATRIX_HEIGHT - 1; i++)
	{
		gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(i, MATRIX_WIDTH - 1));
	}

	gameManager.addBlock(&gameManager.textureManager.gGreySquare, MatrixPoint(MATRIX_HEIGHT - 1, MATRIX_WIDTH - 1));
}

/**
 * XX
 * OO
 * OO
 * 
 */

void generateWorldasdfsd()
{
	gameManager.addBlock(&gameManager.textureManager.gGreySquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gGreySquare, MatrixPoint(1, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gGreySquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	gameManager.addBlock(&gameManager.textureManager.gGreySquare, MatrixPoint(1, MATRIX_WIDTH - 2));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(2, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(2, MATRIX_WIDTH - 2));
}

/*
 * OOOOOOOOOOOOOOOOOOOOOOOOOOO
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 */
void generateWorld()
{
	for (int i = 0; i < MATRIX_HEIGHT - 1; i++)
	{
		for (int j = 0; j < MATRIX_WIDTH; j++)
		{
			gameManager.addBlock(&gameManager.textureManager.gRedSquare, MatrixPoint(i, j));
		}
	}
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(MATRIX_HEIGHT - 1, j));
	}
}

/**
 * OOOOOOOOOOOOOOOOOO
 * XXXXXXXXXXXXXXXXXX
 */
void generateWorldada()
{
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gRedSquare, MatrixPoint(0, j));
	}
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(1, j));
	}
}

/*
 * X
 * X
 * X
 * X
 * O
 * O
 */
void generateWorldasdasd()
{
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(1, MATRIX_WIDTH - 1));
	for (int i = 2; i < MATRIX_HEIGHT; i++)
	{
		gameManager.addBlock(&gameManager.textureManager.gRedSquare, MatrixPoint(i, MATRIX_WIDTH - 1));
	}
}


/**
 * V 
 * O
 * OXX
 */
void generateWorld6696()
{
	gameManager.addBlock(&gameManager.textureManager.gYellowSquare, MatrixPoint(2, MATRIX_WIDTH - 3));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
	gameManager.addBlock(&gameManager.textureManager.gGreenSquare, MatrixPoint(1, MATRIX_WIDTH - 3));
	gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	gameManager.addBlock(&gameManager.textureManager.gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
}

// Generate a random rectangle of blocks starting at the game first column
void generateWorldasdfasdf()
{
	for (int j = GAME_FIRST_COLUMN; j < MATRIX_WIDTH; j++)
	{
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			gameManager.addBlock(gameManager.textureManager.randomTexture(), MatrixPoint(i, j));
		}
	}
}
