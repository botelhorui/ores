#pragma once
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
#include "screens.h"
class StartScreen;
class LBlock;


class ScreensManager
{
	
};
class TextureManager
{
public:
	bool loadMedia();

	/* Generates a random texture from the available ones*/
	LTexture* randomTexture();
	void close();

	//Scene texture
	LTexture gRedSquare;
	LTexture gGreySquare;
	LTexture gGreenSquare;
	LTexture gBlueSquare;
	LTexture gYellowSquare;

	// Buttons and text
	LTexture gOresLogo;
	LTexture gPlayButton;
	LTexture gGameOverLogo;
	LTexture gPlayAgainButton;

	// All block textures
	std::vector<LTexture*> blocksTextures;
	std::vector<LTexture*> propsTextures;
	

};

class SoundManager
{
public:
	bool loadMedia() {
		bool success=true;
		std::string basePath = "../resources/";

		//Load sound effects
		auto path = basePath + "metal.click.wav";
		gClickBlockSound = Mix_LoadWAV(path.c_str());
		if (gClickBlockSound == nullptr)
		{
			printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}
		//Load sound effects
		path = basePath + "slidding.wav";
		gColumnInsertionSound = Mix_LoadWAV(path.c_str());
		if (gColumnInsertionSound == nullptr)
		{
			printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}
		//Load sound effects
		path = basePath + "gameover.wav";
		gGameOverSound = Mix_LoadWAV(path.c_str());
		if (gGameOverSound == nullptr)
		{
			printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
			success = false;
		}
		return success;
	}

	void close()
	{
		Mix_FreeChunk(gClickBlockSound);
		Mix_FreeChunk(gGameOverSound);
		Mix_FreeChunk(gColumnInsertionSound);
		gClickBlockSound = nullptr;
		gGameOverSound = nullptr;
		gColumnInsertionSound = nullptr;
	}
	//The sound effects that will be used
	Mix_Chunk* gClickBlockSound = nullptr;
	Mix_Chunk* gColumnInsertionSound = nullptr;
	Mix_Chunk* gGameOverSound = nullptr;
};

