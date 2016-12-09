#pragma once
#include <SDL.h>
#include <cstdlib>
#include "managers.h"
#include "points.h"

class LTexture;

// utility
inline double fRand(double fMin, double fMax)
{
	double f = double(rand()) / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

// Converts
inline PixelPoint MatrixToPixelPoint(MatrixPoint mp)
{
	int y = SCREEN_HEIGHT - (MATRIX_BOT_BORDER + 1 + mp.i) * TEXTURE_SIDE;
	int x = TEXTURE_SIDE * (MATRIX_LEFT_BORDER + mp.j);
	return PixelPoint(x, y);
}

void generateWorld();



extern GameManager gameManager;
