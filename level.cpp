#include "constants.h"
#include "geral.h"

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
		addBlock(&gBlueSquare, MatrixPoint(0, j));
	}
}

/**
 * O##
 */
void generateWorldadas()
{
	addBlock(&gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	addBlock(&gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
}

/**
* O########
*/

void generateWorldasf()
{
	addBlock(&gBlueSquare, MatrixPoint(0, 0));
	for (int j = 1; j < MATRIX_WIDTH; j++)
	{
		addBlock(&gGreenSquare, MatrixPoint(0, j));
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
		addBlock(&gBlueSquare, MatrixPoint(i, 0));
	}
	for (int j = 1; j < MATRIX_WIDTH; j++)
	{
		addBlock(&gGreenSquare, MatrixPoint(0, j));
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
			addBlock(&gBlueSquare, MatrixPoint(i, j));
		}
	}
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
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
		addBlock(&gGreenSquare, MatrixPoint(i, MATRIX_WIDTH - 1));
	}

	addBlock(&gGreySquare, MatrixPoint(MATRIX_HEIGHT - 1, MATRIX_WIDTH - 1));
}

/**
 * XX
 * OO
 * OO
 * 
 */

void generateWorldasdfsd()
{
	addBlock(&gGreySquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	addBlock(&gGreySquare, MatrixPoint(1, MATRIX_WIDTH - 1));
	addBlock(&gGreySquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	addBlock(&gGreySquare, MatrixPoint(1, MATRIX_WIDTH - 2));
	addBlock(&gGreenSquare, MatrixPoint(2, MATRIX_WIDTH - 1));
	addBlock(&gGreenSquare, MatrixPoint(2, MATRIX_WIDTH - 2));
}

/*
 * OOOOOOOOOOOOOOOOOOOOOOOOOOO
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 * XXXXXXXXXXXXXXXXXXXXXXXXXXX
 */
void generateWorldfhfhf()
{
	for (int i = 0; i < MATRIX_HEIGHT - 1; i++)
	{
		for (int j = 0; j < MATRIX_WIDTH; j++)
		{
			addBlock(&gRedSquare, MatrixPoint(i, j));
		}
	}
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		addBlock(&gGreenSquare, MatrixPoint(MATRIX_HEIGHT - 1, j));
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
		addBlock(&gRedSquare, MatrixPoint(0, j));
	}
	for (int j = 0; j < MATRIX_WIDTH; j++)
	{
		addBlock(&gGreenSquare, MatrixPoint(1, j));
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
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
	addBlock(&gGreenSquare, MatrixPoint(1, MATRIX_WIDTH - 1));
	for (int i = 2; i < MATRIX_HEIGHT; i++)
	{
		addBlock(&gRedSquare, MatrixPoint(i, MATRIX_WIDTH - 1));
	}
}


/**
 * V 
 * O
 * OXX
 */
void generateWorld6696()
{
	addBlock(&gYellowSquare, MatrixPoint(2, MATRIX_WIDTH - 3));
	addBlock(&gGreenSquare, MatrixPoint(0, MATRIX_WIDTH - 3));
	addBlock(&gGreenSquare, MatrixPoint(1, MATRIX_WIDTH - 3));
	addBlock(&gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 2));
	addBlock(&gBlueSquare, MatrixPoint(0, MATRIX_WIDTH - 1));
}

// Generate a random rectangle of blocks starting at the game first column
void generateWorld()
{
	for (int j = GAME_FIRST_COLUMN; j < MATRIX_WIDTH; j++)
	{
		for (int i = 0; i < MATRIX_HEIGHT; i++)
		{
			addBlock(randomTexture(), MatrixPoint(i, j));
		}
	}
}
