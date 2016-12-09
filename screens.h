#pragma once
#include <SDL.h>


class GameManager;

class Screen
{
public:
	Screen(GameManager* gameManager);
	virtual ~Screen();

	virtual void handleEvent(SDL_Event* e) = 0;
	virtual void render() = 0;
	GameManager* gameManager;
};



class StartScreen : public Screen
{
public:
	StartScreen(GameManager* gameManager);

	void handleEvent(SDL_Event* e) override;

	void render() override;

private:
	bool beingClicked;
};


class GameoverScreen : public Screen
{
public:
	GameoverScreen(GameManager* gameManager);

	void handleEvent(SDL_Event* e) override;

	void render() override;

private:
	bool beingClicked;
};