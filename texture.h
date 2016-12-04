#pragma once

#include <SDL.h>
#include <string>

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
	void setColor(Uint8 red, Uint8 green, Uint8 blue) const;

	//Set blending
	void setBlendMode(SDL_BlendMode blending) const;

	//Set alpha modulation
	void setAlpha(Uint8 alpha) const;

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0, SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

	//Gets image dimensions
	int getWidth() const;
	int getHeight() const;

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};
