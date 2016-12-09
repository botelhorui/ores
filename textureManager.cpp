#include <SDL.h> //http://libsdl.org/download-2.0.php
#include <SDL_image.h> //https://www.libsdl.org/projects/SDL_image/
#include <SDL_mixer.h> //https://www.libsdl.org/projects/SDL_mixer/


#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "texture.h"
#include "block.h"
#include "managers.h"
#include "gamemanager.h"

bool TextureManager::loadMedia()
{
	//Loading success flag
	bool success = true;
	std::string basePath = "../resources/";

	//Load squares
	if (!gRedSquare.loadFromFile(basePath + "red.square.bmp"))
	{
		printf("Failed to load red square texture!\n");
		success = false;
	}
	blocksTextures.push_back(&gRedSquare);
	if (!gGreySquare.loadFromFile(basePath + "grey.square.bmp"))
	{
		printf("Failed to load grey square texture!\n");
		success = false;
	}
	blocksTextures.push_back(&gGreySquare);
	if (!gGreenSquare.loadFromFile(basePath + "green.square.bmp"))
	{
		printf("Failed to load green square texture!\n");
		success = false;
	}
	blocksTextures.push_back(&gGreenSquare);
	if (!gBlueSquare.loadFromFile(basePath + "blue.square.bmp"))
	{
		printf("Failed to load blue square texture!\n");
		success = false;
	}
	blocksTextures.push_back(&gBlueSquare);
	if (!gYellowSquare.loadFromFile(basePath + "yellow.square.bmp"))
	{
		printf("Failed to load yellow square texture!\n");
		success = false;
	}
	blocksTextures.push_back(&gYellowSquare);

	// Load Text and buttons
	if (!gOresLogo.loadFromFile(basePath + "ores.logo.bmp"))
	{
		printf("Failed to load ores logo texture!\n");
		success = false;
	}
	propsTextures.push_back(&gOresLogo);

	if (!gPlayButton.loadFromFile(basePath + "play.button.bmp"))
	{
		printf("Failed to load play button texture!\n");
		success = false;
	}
	propsTextures.push_back(&gPlayButton);
	if (!gGameOverLogo.loadFromFile(basePath + "gameover.bmp"))
	{
		printf("Failed to load gameover texture!\n");
		success = false;
	}
	propsTextures.push_back(&gGameOverLogo);
	if (!gPlayAgainButton.loadFromFile(basePath + "play.again.button.bmp"))
	{
		printf("Failed to load play again texture!\n");
		success = false;
	}
	propsTextures.push_back(&gPlayAgainButton);
	return success;
}

/* Generates a random texture from the available ones*/
LTexture* TextureManager::randomTexture()
{
	// http://stackoverflow.com/questions/5008804/generating-random-integer-from-a-range
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, blocksTextures.size() - 1);
	int randomTexture = uni(rng);
	return blocksTextures[randomTexture];
}


void TextureManager::close()
{
	//Free loaded textures
	for (auto tex : blocksTextures)
		tex->free();

	for (auto tex : propsTextures)
		tex->free();

	propsTextures.clear();
	blocksTextures.clear();

}

