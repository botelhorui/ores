#pragma once
#include <SDL.h>

#include "texture.h"
#include "geral.h"
#include "points.h"


//#include "common.h"

class LBlock
{
public:

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


	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	Uint32 getAlpha() const;

	// VX
	int velocityX = 0;

	// VY
	int velocityY = 0;

	//
	int angularVelocity = 0;

	// Rotation of the block, used for the animation of falling
	int degrees = 0;

	// true when doing depth first search and had already been tested if the texture matches with adjacent block
	bool searched = false;

	// True when the block is falling after some of the blocks below were matched
	bool falling = false;

	// True when there is an empty column and the block is moving to the right to occupy that collumn
	bool sliding = false;

private:
	//Alpha Modulation component
	Uint8 alpha = 255;

	LTexture* mTex = nullptr;

	// Indicates that the block had the cursor over it and therefore it is highlighted
	bool cursorHovering = false;

	// Indicates if the mouse is pressing and holding the mouse 1 inside the block
	bool blockBeingClicked = false;

	// the block matched the same color of adjacent clicked blocks
	bool removed = false;

	// Position in the SDL screen
	PixelPoint pixelPoint = PixelPoint(0, 0);

	// Position in the game matrix
	MatrixPoint matrixPoint = MatrixPoint(0, 0);
};
