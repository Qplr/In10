#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"

class EasyVector: public sf::Vector2i
{
public:
	operator long long() const;
	operator sf::Vector2f() const;
	EasyVector() {}
	EasyVector(int x, int y) : sf::Vector2i(x, y) {}
	EasyVector(const sf::Vector2i& vec) : sf::Vector2i(vec) {}

	inline int x_plus_y() const { return x + y; }
	friend inline EasyVector operator-(const EasyVector& first, const EasyVector& second);
};

