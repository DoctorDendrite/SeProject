#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <Windows.h>

#include "Game.h"

using namespace sf;

int main()
{
	// Temp rand (use better formula)
	srand(time(NULL));
	
	bool isFullscreen = true;

	RenderWindow window(VideoMode(1920, 1080), "360 Shooter!", Style::Default | sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(false);
	window.setMouseCursorVisible(false);
	
	Game::GameSequence(window, isFullscreen);

	return 0;
}
