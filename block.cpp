#include "block.h"
#include "common.h"


LBlock::LBlock() {
}

void LBlock::setRenderPosition(int x, int y) {
	pos.x = x;
	pos.y = y;
}

void LBlock::setMatrixPosition(int j, int i) {
	matrixPos.x = j;
	matrixPos.y = i;
}

void LBlock::setTexture(LTexture* tex) {
	mTex = tex;
}

LTexture* LBlock::getTexture() {
	return mTex;
}

void LBlock::handleEvent(SDL_Event* e) {
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
		if (x < pos.x) {
			mouseInsideBlock = false;
		}
		// if its to the right of the block
		else if (x > pos.x + mTex->getWidth() - 1) { // subtract 1 to avoid overlap with the block to the right
			mouseInsideBlock = false;
		}
		// if its above
		else if (y < pos.y) {
			mouseInsideBlock = false;
		}
		// if its under
		else if (y > pos.y + mTex->getHeight() - 1) { // subtract 1 to avoid overlap with the bottom block
			mouseInsideBlock = false;
		}

		if (mouseInsideBlock) {
			if (e->type == SDL_MOUSEBUTTONDOWN) {
				blockBeingClicked = true;
			}
			else if (blockBeingClicked && e->type == SDL_MOUSEBUTTONUP) { // if block is clicked				
				handleBlockClick(this);
				blockBeingClicked = false;
			}
		}
		highlight = mouseInsideBlock;
	}
}

void LBlock::setDegrees(int a) {
	degrees = a;
}

void LBlock::render() {
	if (removed)
		return;
	mTex->render(pos.x, pos.y, NULL, degrees);
	// render hightlight
	if (highlight) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		// Left side rectangle
		SDL_Rect fillRect = { pos.x, pos.y, HIGHLIGHT_BORDER_WIDTH, mTex->getHeight() };
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Right side rectangle
		fillRect = { pos.x + mTex->getWidth() - HIGHLIGHT_BORDER_WIDTH , pos.y, HIGHLIGHT_BORDER_WIDTH, mTex->getHeight() };
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Top side rectangle
		fillRect = { pos.x, pos.y, mTex->getWidth(), HIGHLIGHT_BORDER_WIDTH };
		SDL_RenderFillRect(gRenderer, &fillRect);
		// Bottom side rectangle
		fillRect = { pos.x, pos.y + mTex->getHeight() - HIGHLIGHT_BORDER_WIDTH, mTex->getWidth(), HIGHLIGHT_BORDER_WIDTH };
		SDL_RenderFillRect(gRenderer, &fillRect);
	}
}