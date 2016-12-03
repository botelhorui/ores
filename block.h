#pragma once
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

class LBlock
{
public:
	LBlock();

	void setRenderPosition(int x, int y);
	void setMatrixPosition(int j, int i);
	void setDegrees(int a);
	void setTexture(LTexture* tex);
	LTexture* getTexture();
	void handleEvent(SDL_Event* e);
	void render();
	// Position in the screen
	SDL_Point pixelPos;
	SDL_Point matrixPos;
	int degrees;

	bool searched; // true when doing depth first search

	bool falling;
private:

	LTexture* mTex;
	bool highlight = false;
	bool blockBeingClicked = false;
	bool removed = false;


};