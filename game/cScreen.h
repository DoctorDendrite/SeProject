#pragma once

#include <iostream>
#include <SFML\Graphics.hpp>

#define pass(name, value) std::cout << name << ": [" << value << "]\n"
#define pause(name, value) pass(name, value); std::cin.get()

class cScreen
{
public:
    virtual int Run(sf::RenderWindow& App) = 0;
};
