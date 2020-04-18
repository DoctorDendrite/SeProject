#pragma once

// Source:
//    https://github.com/SFML/SFML/wiki/Tutorial:-Manage-different-Screens
//    
// Retrieved:
//    2020_04_17

#include <SFML/Graphics.hpp>
#include <iostream>
#include "cScreen.h"
#include "Game.h"

const float INIT_X_POS = 280.f;
const float INIT_Y_POS = 160.f;
const float ITEM_HEIGHT = 60.f;
const int DEFAULT_POINT = 20;
const char* FONT_FILE_PATH = "res/verdanab.ttf";

static sf::Font MY_FONT;

class screen_0 : public cScreen
{
private:
	int alpha_max;
	int alpha_div;
	bool playing;
public:
	screen_0(void);
	virtual int Run(sf::RenderWindow& App);
};

screen_0::screen_0(void)
{
	alpha_max = 3 * 255;
	alpha_div = 3;
	playing = false;
}

enum class MENU_ITEMS
{
	PLAY,
	LOAD,
	SAVE,
	EXIT,
	COUNT
};

int Prev(int index, int modulus)
{
	if (index == 0)
		return modulus;

	return index - 1;
}

int Next(int index, int modulus)
{
	if (index == modulus)
		return 0;

	return index + 1;
}

int PrevItemIndex(int index)
{
	return Prev(index, (int)MENU_ITEMS::COUNT - 1);
}

int NextItemIndex(int index)
{
	return Next(index, (int)MENU_ITEMS::COUNT - 1);
}

typedef std::vector<sf::Text> Menu;

#define SET_MENU_ITEM(item, displayStr, myFont, point, xPos, yPos) \
item.setFont(myFont); \
item.setCharacterSize(point); \
item.setString(displayStr); \
item.setPosition({ xPos, yPos })

#define SET_MENU_ITEM_DEFAULT(item, displayStr, yPos) \
SET_MENU_ITEM(item, displayStr, MY_FONT, DEFAULT_POINT, INIT_X_POS, yPos)

#define NEXT_Y_POS(size) INIT_Y_POS + (size - 1) * ITEM_HEIGHT

#define ADD_MENU_ITEM(menu, displayStr) \
menu.push_back(sf::Text()); \
SET_MENU_ITEM_DEFAULT(menu.back(), displayStr, NEXT_Y_POS(menu.size()))

void IndicateItem(Menu& myMenu, int itemIndex)
{
	for (size_t i = 0; i < itemIndex; ++i)
		myMenu[i].setFillColor(sf::Color(255, 0, 0, 255));

	if (itemIndex < myMenu.size())
		myMenu[itemIndex].setFillColor(sf::Color(255, 255, 255, 255));

	for (size_t i = itemIndex + 1; i < myMenu.size(); ++i)
		myMenu[i].setFillColor(sf::Color(255, 0, 0, 255));
}

int screen_0::Run(sf::RenderWindow& App)
{
	sf::Event event;
	bool running = true;
	sf::Texture myTexture;
	sf::Sprite mySprite;
	int alpha = 0;
	Menu mainMenu;

	// if (!Texture.loadFromFile("presentation.png"))
	// {
	// 	std::cerr << "Error loading presentation.gif" << std::endl;
	// 	return (-1);
	// }

	mySprite.setTexture(myTexture);
	mySprite.setColor(sf::Color(255, 255, 255, alpha));

	if (!MY_FONT.loadFromFile(FONT_FILE_PATH))
	{
		std::cerr << "Error loading verdanab.ttf" << std::endl;
		return (-1);
	}

	ADD_MENU_ITEM(mainMenu, "Play");
	ADD_MENU_ITEM(mainMenu, "Load");
	ADD_MENU_ITEM(mainMenu, "Save");
	ADD_MENU_ITEM(mainMenu, "Exit");

	sf::Text label;
	SET_MENU_ITEM_DEFAULT(label, Game::CollisionsToString(), NEXT_Y_POS(mainMenu.size() + 1));

	if (playing)
		alpha = alpha_max;

	int index = 0;

	while (running)
	{
		// Verifying events
		while (App.pollEvent(event))
		{
			// Window closed
			if (event.type == sf::Event::Closed)
				return (-1);

			// Key pressed
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
				case sf::Keyboard::W:
					index = PrevItemIndex(index);
					break;
				case sf::Keyboard::Down:
				case sf::Keyboard::S:
					index = NextItemIndex(index);
					break;
				case sf::Keyboard::Return:
					switch ((MENU_ITEMS) index)
					{
					case MENU_ITEMS::PLAY:
						// playing = true;
						mainMenu[(int)MENU_ITEMS::PLAY].setString("Continue");
						return (1);

					case MENU_ITEMS::LOAD:
						break;

					case MENU_ITEMS::SAVE:
						break;

					case MENU_ITEMS::EXIT:
						return (-1);
					}

					break;
				default:
					break;
				}
			}
		}

		// When getting at alpha_max, we stop modifying the sprite
		if (alpha < alpha_max)
			alpha++;

		mySprite.setColor(sf::Color(255, 255, 255, alpha / alpha_div));
		IndicateItem(mainMenu, index);

		// Clearing screen
		App.clear();
		
		// Drawing
		App.draw(mySprite);
		
		if (alpha == alpha_max)
			for (auto item : mainMenu)
				App.draw(item);
		
		App.draw(label);
		App.display();
	}

	// Never reaching this point normally, but just in case, exit the application
	return (-1);
}
