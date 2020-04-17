#include <SFML/Graphics.hpp>
#include <iostream>
#include "screens.h"

using namespace sf;

int main(int argc, char** argv)
{
	// Applications variables
	std::vector<cScreen*> Screens;
	int screen = 0;

	// // Window creation
	// sf::RenderWindow App(sf::VideoMode(640, 480, 32), "SFML Demo 3");
	RenderWindow App(VideoMode(1920, 1080), "360 Shooter!", Style::Default | sf::Style::Fullscreen);

	// Mouse cursor no more visible
	App.setMouseCursorVisible(false);

	// Screens preparations
	screen_0 s0;
	Screens.push_back(&s0);
	screen_1 s1(App);
	Screens.push_back(&s1);

	// Main loop
	while (screen >= 0)
	{
		screen = Screens[screen]->Run(App);
	}

	return EXIT_SUCCESS;
}
