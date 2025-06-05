#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"

class EasyVector : public sf::Vector2i
{
public:
	operator sf::Vector2f();
	EasyVector() {}
	EasyVector(int x, int y) : sf::Vector2i(x, y) {}
	EasyVector(const sf::Vector2i& vec) : sf::Vector2i(vec) {}
	EasyVector(const sf::Vector2f& vec) : sf::Vector2i(vec) {}

	friend bool operator<(const EasyVector& lhs, const EasyVector& rhs);
};

