//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>


//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int MATRIX_WIDTH = 17;

const int MATRIX_LEFT_BORDER = 2;

const int MATRIX_HEIGHT = 12;
const int MATRIX_BOT_BORDER = 1;

const int GAME_FIRST_COLUMN = 8;

const int HIGHLIGHT_BORDER_WIDTH = 3;

const int TEXTURE_SIDE = 32;

int matrixToPosCoordsJX(int j) {
	return TEXTURE_SIDE * (MATRIX_LEFT_BORDER + j);
}
int matrixToPosCoordsIY(int i) {
	return SCREEN_HEIGHT - (MATRIX_BOT_BORDER + 1 + i) * TEXTURE_SIDE;
}

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

void drawBackground() {
	SDL_SetRenderDrawColor(gRenderer, 244, 173, 66, 0xFF);
	SDL_RenderClear(gRenderer);
}

//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}


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
	SDL_Point pos;
	SDL_Point matrixPos;
	int degrees;

	bool searched; // true when doing depth first search

	bool falling;
private:

	LTexture* mTex;
	bool highlight = false;
	bool beingClicked = false;
	bool removed = false;


};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

void handleBlockClick(LBlock* block);


//Scene texture
LTexture gRedSquare;
LTexture gGreySquare;
LTexture gGreenSquare;
LTexture gBlueSquare;
LTexture gYellowSquare;

std::vector<LTexture*> textures;

bool piecesAreFalling = false;

//Buttons

LBlock* matrix[MATRIX_WIDTH][MATRIX_HEIGHT];

std::vector<LBlock*> blocks;
std::vector<LBlock*> removedBlocks;

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
		bool inside = true;

		if (x < pos.x) {
			inside = false;
		}
		else if (x > pos.x + mTex->getWidth()-1) { // avoid overlap
			inside = false;
		}
		else if (y < pos.y) {
			inside = false;
		}
		else if (y > pos.y + mTex->getHeight()-1) { // avoid overlap
			inside = false;
		}

		if (inside) {
			if (e->type == SDL_MOUSEBUTTONDOWN) {
				beingClicked = true;
			}else if (beingClicked && e->type == SDL_MOUSEBUTTONUP) {
				//removed = beingClicked && twoOrMoreEqual(mTex);
				handleBlockClick(this);
				beingClicked = false;
			}			
		}
		highlight = inside;
	}
}

void markFalling(LBlock* block) {
	for (int i = block->matrixPos.y; i < MATRIX_HEIGHT; i++) {
		LBlock* el = matrix[block->matrixPos.x][i];
		if (el != NULL) {
			el->falling = true;
		}
	}	
}

void sweep(LBlock* block, LBlock* other) {
	if (other != NULL && block->getTexture() == other->getTexture()) {
		if (removedBlocks.size() == 0 || // size zero means the search just started
			removedBlocks.back() != block) // to avoid duplicates
			removedBlocks.push_back(block);
		markFalling(block);
		removedBlocks.push_back(other);
		markFalling(other);
		handleBlockClick(other);
		piecesAreFalling = true;
	}
}


void handleBlockClick(LBlock* block) {
	if (block->searched)
		return;
	block->searched = true;
	// handle the top block
	if (block->matrixPos.y < MATRIX_HEIGHT - 1) {
		LBlock* top = matrix[block->matrixPos.x][block->matrixPos.y + 1];
		sweep(block, top);		
	}
	if (block->matrixPos.y > 0) {
		LBlock* bot = matrix[block->matrixPos.x][block->matrixPos.y - 1];
		sweep(block, bot);
	}
	if (block->matrixPos.x < MATRIX_WIDTH - 1) {
		LBlock* right = matrix[block->matrixPos.x+1][block->matrixPos.y];
		sweep(block, right);
	}
	if (block->matrixPos.x > 0) {
		LBlock* left = matrix[block->matrixPos.x-1][block->matrixPos.y];
		sweep(block, left);
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

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Ores", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load squares
	if (!gRedSquare.loadFromFile("resources/red.square.bmp"))
	{
		printf("Failed to load red square texture!\n");
		success = false;
	}
	textures.push_back(&gRedSquare);
	if (!gGreySquare.loadFromFile("resources/grey.square.bmp"))
	{
		printf("Failed to load grey square texture!\n");
		success = false;
	}
	textures.push_back(&gGreySquare);
	if (!gGreenSquare.loadFromFile("resources/green.square.bmp"))
	{
		printf("Failed to load green square texture!\n");
		success = false;
	}
	textures.push_back(&gGreenSquare);
	if (!gBlueSquare.loadFromFile("resources/blue.square.bmp"))
	{
		printf("Failed to load blue square texture!\n");
		success = false;
	}
	textures.push_back(&gBlueSquare);
	if (!gYellowSquare.loadFromFile("resources/yellow.square.bmp"))
	{
		printf("Failed to load yellow square texture!\n");
		success = false;
	}
	textures.push_back(&gYellowSquare);
	return success;
}

void close()
{
	//Free loaded textures
	for (LTexture* tex : textures)
		tex->free();

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool processFalling() {
	int startTime = SDL_GetTicks();
	SDL_Event e;
	bool quit = false;
	int blocksFalling = 0;

	// Calculate the new matrix coordinates of each faling block
	for (int j = 0; j < MATRIX_WIDTH; j++) {
		int lowest = 0;
		for (int i = 0; i < MATRIX_HEIGHT; i++) {
			if (matrix[j][i] != NULL) {
				LBlock* block = matrix[j][i];				
				matrix[j][i] = NULL;
				matrix[j][lowest] = block;
				block->setMatrixPosition(j, lowest);				
				//block->setRenderPosition(matrixToPosCoordsJX(j), matrixToPosCoordsIY(lowest));	
				if (lowest != i) {
					blocksFalling++;
				}
				lowest++;
			}			
		}
	}
	

	while (blocksFalling > 0 && !quit ) {
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return quit;
			}
		}

		double dt = (SDL_GetTicks() - startTime) / 1000.0;

		for (LBlock* it : blocks) {
			if (it->falling) {		
				it->pos.y = it->pos.y + 10*dt + 5*dt*dt;	
				int yTarget = matrixToPosCoordsIY(it->matrixPos.y);
				if (it->pos.y >= yTarget) {
					it->falling = false;
					blocksFalling--;
					it->pos.y = yTarget;
				}
			}
		}

		//Background color
		drawBackground();

		//Render blocks
		for (LBlock* it : blocks) {
			it->render();
		}
		//Update screen
		SDL_RenderPresent(gRenderer);

	}
	int blocksSliding = 0;
	int highest = MATRIX_WIDTH-1;
	for (int j = highest; j > 0; j--) {
		if (matrix[j][0] != NULL) {
			if (j != highest) { // means j has empty collumn to the right				
				for (int i = 0; i < MATRIX_HEIGHT; i++) {
					if (matrix[j][i] != NULL) {
						LBlock* block = matrix[j][i];
						blocksSliding++;
						block->falling = true;
						matrix[j][i] = NULL;
						matrix[highest][i] = block;
						block->setMatrixPosition(highest, i);
					}
				}				
			}
			highest--;
		}
			
	}

	while (blocksSliding > 0 && !quit) {
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return quit;
			}
		}
		double dt = (SDL_GetTicks() - startTime) / 1000.0;

		for (LBlock* it : blocks) {
			if (it->falling) {
				it->pos.x = it->pos.x + 10 * dt + 5 * dt*dt;
				int xTarget = matrixToPosCoordsJX(it->matrixPos.x);
				if (it->pos.x >= xTarget) {
					it->falling = false;
					blocksSliding--;
					it->pos.x = xTarget;
				}
			}
		}

		//Background color
		drawBackground();

		//Render blocks
		for (LBlock* it : blocks) {
			it->render();
		}
		//Update screen
		SDL_RenderPresent(gRenderer);
	}

	return quit;
}

void addBlock(LTexture* tex, int j, int i) {
	LBlock* blk = new LBlock();
	blk->setTexture(tex);
	blk->setMatrixPosition(j, i);
	blk->setRenderPosition(matrixToPosCoordsJX(j), matrixToPosCoordsIY(i));
	matrix[j][i] = blk;
	blocks.push_back(blk);
}
void generateWorld2() {
	int j = GAME_FIRST_COLUMN;

	addBlock(&gGreenSquare, j, 1);
	addBlock(&gGreenSquare, j, 2);
	addBlock(&gGreySquare, j, 3);
	addBlock(&gRedSquare, j++, 0);
	for (; j < MATRIX_WIDTH; j++)
		addBlock(&gBlueSquare, j, 0);
}
void generateWorld() {	
	// http://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, textures.size() - 1);

	for (int j = GAME_FIRST_COLUMN; j < MATRIX_WIDTH; j++) {
		for (int i = 0; i < MATRIX_HEIGHT; i++) {
			int randomTexture = uni(rng);
			addBlock(textures[randomTexture], j, i);
		}
	}
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
			return -1;
		}
		else
		{
			for (int i = 0; i < MATRIX_HEIGHT; i++)
				for (int j = 0; j < MATRIX_WIDTH; j++)
					matrix[i][j] = NULL;

			generateWorld();

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{			
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}					
					for (LBlock* it : blocks) {
						
						for (auto b : blocks)
							b->searched = false;

						it->handleEvent(&e);
					}
				}
				// handle removed blocks
				for (LBlock* it : removedBlocks) {
					matrix[it->matrixPos.x][it->matrixPos.y] = NULL;
					blocks.erase(std::remove(blocks.begin(), blocks.end(), it), blocks.end());
				}
				removedBlocks.clear();

				drawBackground();

				//Render blocks
				for (LBlock* it : blocks) {
					it->render();
				}
				//Update screen
				SDL_RenderPresent(gRenderer);

				if (piecesAreFalling) {
					quit = processFalling();
					piecesAreFalling = false;
				}
				
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}