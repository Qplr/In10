#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"

class EasyVector: public sf::Vector2i
{
public:
	operator long long() const;
	EasyVector(int x, int y) : sf::Vector2i(x, y) {}
	EasyVector(const sf::Vector2i& vec) : sf::Vector2i(vec) {}
};

