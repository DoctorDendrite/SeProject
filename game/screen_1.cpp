
#include "screen_1.h"
#include "Game.h"

screen_1::screen_1(sf::RenderWindow& App)
{
	movement_step = 5;
	posx = 320;
	posy = 240;
	//Setting sprite
	Rectangle.setFillColor(sf::Color(255, 255, 255, 150));
	Rectangle.setSize({ 10.f, 10.f });

	Game::Initialize(App);
}

int screen_1::Run(sf::RenderWindow& App)
{
	return Game::GameSequence(App, 1);
}
