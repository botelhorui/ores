#pragma once
#include <SDL.h>

#include "texture.h"
#include "structs.h"


//#include "common.h"

class LBlock
{
public:
	// Initializes variables
	LBlock();

	// Handles mouse events
	void handleEvent(SDL_Event* e);

	// Renders the block at the render position
	void render() const;

	/*
	// set the pixel position of the top left of the block
	void setPixelPos(int x, int y);

	// set the position of the block in the game matrix
	void setMatrixPos(int j, int i);
	*/

	// Sets the texture rendered for this block
	void setTexture(LTexture* tex);

	// Gets the associated texture
	LTexture* getTexture() const;
	
	// Set the rotation of the block, used when the blocks are collected and fall to the ground
	void setDegrees(int a);

	PixelPoint getPixelPoint() const;

	MatrixPoint getMatrixPoint() const;
	

	void setPixelPoint(PixelPoint pp);

	void setMatrixPoint(MatrixPoint mp);

	/*
	SDL_Point pixelPos;	
	SDL_Point matrixPos;
	*/

	// Rotation of the block, used for the animation of falling
	int degrees;

	// true when doing depth first search and had already been tested if the texture matches with adjacent block
	bool searched; 

	// True when the block is falling after some of the blocks below were matched
	bool falling;

	// True when there is an empty column and the block is moving to the right to occupy that collumn
	bool sliding;

private:

	LTexture* mTex;

	// Indicates that the block had the cursor over it and therefore it is highlighted
	bool highlight;

	// Indicates if the mouse is pressing and holding the mouse 1 inside the block
	bool blockBeingClicked;

	// the block matched the same color of adjacent clicked blocks
	bool removed;

	// Position in the SDL screen
	PixelPoint pixelPoint;

	// Position in the game matrix
	MatrixPoint matrixPoint;
};
