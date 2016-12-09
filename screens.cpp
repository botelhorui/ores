#include "gamemanager.h"
#include "screens.h"


Screen::Screen(GameManager* gm) : gameManager(gm) {}

Screen::~Screen()
{
}

StartScreen::StartScreen(GameManager* gameManager) : Screen(gameManager), beingClicked(false)
{
}

void StartScreen::handleEvent(SDL_Event* e)
{
	if (!gameManager->gCurrentScreenStartScreen)
		return;

	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			beingClicked = true;
		}
		else if (beingClicked && e->type == SDL_MOUSEBUTTONUP)
		{ // if block is clicked	
		  // change screen to game loop
			gameManager->gCurrentScreenStartScreen = false;
			gameManager->gCurrentScreenGame = true;
			beingClicked = false;
		}
	}
}

void StartScreen::render()
{
	// background color
	SDL_SetRenderDrawColor(gameManager->gRenderer, 244, 173, 66, 0xFF);
	SDL_RenderClear(gameManager->gRenderer);
	int x = int(SCREEN_WIDTH / 2.0 - gameManager->textureManager.gOresLogo.getWidth() / 2.0);
	int y = int(SCREEN_HEIGHT / 2.0 - gameManager->textureManager.gOresLogo.getHeight() / 2.0);
	gameManager->textureManager.gOresLogo.render(x, y, nullptr, 0);
	x = int(SCREEN_WIDTH / 2.0 - gameManager->textureManager.gPlayButton.getWidth() / 2.0);
	y = int(SCREEN_HEIGHT / 2.0 - gameManager->textureManager.gPlayButton.getHeight() / 2.0 + 2 * gameManager->textureManager.gOresLogo.getHeight());
	gameManager->textureManager.gPlayButton.render(x, y, nullptr, 0);
	SDL_RenderPresent(gameManager->gRenderer);
}



GameoverScreen::GameoverScreen(GameManager* gameManager) : Screen(gameManager), beingClicked(false)
{
}

void GameoverScreen::handleEvent(SDL_Event* e)
{
	if (!gameManager->gCurrentScreenGameover)
		return;
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		if (e->type == SDL_MOUSEBUTTONDOWN)
		{
			beingClicked = true;
		}
		else if (beingClicked && e->type == SDL_MOUSEBUTTONUP)
		{ // if block is clicked	
		  // change screen to game loop
			gameManager->gCurrentScreenGameover = false;
			gameManager->gCurrentScreenGame = true;
			beingClicked = false;
		}
	}
}

void GameoverScreen::render()
{
	// background color
	SDL_SetRenderDrawColor(gameManager->gRenderer, 100, 100, 100, 0xFF);
	SDL_RenderClear(gameManager->gRenderer);
	int x = int(SCREEN_WIDTH / 2.0 - gameManager->textureManager.gGameOverLogo.getWidth() / 2.0);
	int y = int(SCREEN_HEIGHT / 2.0 - gameManager->textureManager.gGameOverLogo.getHeight() / 2.0);
	gameManager->textureManager.gGameOverLogo.render(x, y, nullptr, 0);
	x = int(SCREEN_WIDTH / 2.0 - gameManager->textureManager.gPlayAgainButton.getWidth() / 2.0);
	y = int(SCREEN_HEIGHT / 2.0 - gameManager->textureManager.gPlayAgainButton.getHeight() / 2.0 + 2 * gameManager->textureManager.gGameOverLogo.getHeight());
	gameManager->textureManager.gPlayAgainButton.render(x, y, nullptr, 0);
	SDL_RenderPresent(gameManager->gRenderer);

}

