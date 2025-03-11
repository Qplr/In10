#include "EasyVector.h"

EasyVector::operator long long() const
{
	return (x + y) * (x + y + 1) / 2 + y;
}

EasyVector::operator sf::Vector2f()
{
	return sf::Vector2f(x, y);
}
