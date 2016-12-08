#pragma once
#include <SDL.h>
#include <cstdlib>

class LTexture;

/*
* Represents the coordinates of a pixel in the screen
* the x values grow from left to right of the screen,
* but the y values grow from the top to bottom of the screen
*/
class PixelPoint
{
public:
	PixelPoint(int x, int y);
	int x = 0;
	int y = 0;
};

class MatrixPoint
{
public:
	MatrixPoint(int i, int j);
	int i = 0; // line
	int j = 0; // column
};

// TODO
/*
class GameManager {
SDL_Renderer* gRenderer;
}*/

extern SDL_Renderer* gRenderer;

void addBlock(LTexture* tex, MatrixPoint p);
void generateWorld();
LTexture* randomTexture();

//Scene texture
extern LTexture gRedSquare;
extern LTexture gGreySquare;
extern LTexture gGreenSquare;
extern LTexture gBlueSquare;
extern LTexture gYellowSquare;

inline double fRand(double fMin, double fMax)
{
	double f = double(rand()) / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
