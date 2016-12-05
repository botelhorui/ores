#include "constants.h"
#include "block.h"


void handleBlockClick(LBlock* block);

LBlock::LBlock(): pixelPoint(0, 0), matrixPoint(0, 0)
{
	degrees = 0;
	falling = false;
	searched = false;
	sliding = false;
	mTex = nullptr;
	highlight = false;
	blockBeingClicked = false;
	removed = false;
}

void LBlock::setTexture(LTexture* tex)
{
	mTex = tex;
}

LTexture* LBlock::getTexture() const
{
	return mTex;
}

void LBlock::handleEvent(SDL_Event* e)
{
	if (removed)
		return;

	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		//Check if mouse is inside block picture
		bool mouseInsideBlock = true;

		// if its to the left of the block
		if (x < pixelPoint.x)
		{
			mouseInsideBlock = false;
		}
		// if its to the right of the block
		else if (x > pixelPoint.x + mTex->getWidth() - 1)
		{ // subtract 1 to avoid overlap with the block to the right
			mouseInsideBlock = false;
		}
		// if its above
		else if (y < pixelPoint.y)
		{
			mouseInsideBlock = false;
		}
		// if its under
		else if (y > pixelPoint.y + mTex->getHeight() - 1)
		{ // subtract 1 to avoid overlap with the bottom block
			mouseInsideBlock = false;
		}

		if (mouseInsideBlock)
		{
			if (e->type == SDL_MOUSEBUTTONDOWN)
			{
				blockBeingClicked = true;
			}
			else if (blockBeingClicked && e->type == SDL_MOUSEBUTTONUP)
			{ // if block is clicked				
				handleBlockClick(this);
				blockBeingClicked = false;
			}
		}
		highlight = mouseInsideBlock;
	}
}

void LBlock::setDegrees(int a)
{
	degrees = a;
}

PixelPoint LBlock::getPixelPoint() const
{
	return pixelPoint;
}

MatrixPoint LBlock::getMatrixPoint() const
{
	return matrixPoint;
}

void LBlock::setPixelPoint(PixelPoint pp)
{
	pixelPoint = pp;
}

void LBlock::setMatrixPoint(MatrixPoint mp)
{
	matrixPoint = mp;
}


void LBlock::render() const
{
	if (removed)
		return;
	mTex->render(pixelPoint.x, pixelPoint.y, nullptr, degrees);
	// render hightlight
	if (highlight)
	{
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		// Left side rectangle
		SDL_Rect fillRect = {pixelPoint.x, pixelPoint.y, HIGHLIGHT_BORDER_WIDTH, mTex->getHeight()};
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Right side rectangle
		fillRect = {pixelPoint.x + mTex->getWidth() - HIGHLIGHT_BORDER_WIDTH , pixelPoint.y, HIGHLIGHT_BORDER_WIDTH, mTex->getHeight()};
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Top side rectangle
		fillRect = {pixelPoint.x, pixelPoint.y, mTex->getWidth(), HIGHLIGHT_BORDER_WIDTH};
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Bottom side rectangle
		fillRect = {pixelPoint.x, pixelPoint.y + mTex->getHeight() - HIGHLIGHT_BORDER_WIDTH, mTex->getWidth(), HIGHLIGHT_BORDER_WIDTH};
		SDL_RenderFillRect(gRenderer, &fillRect);
	}
}
