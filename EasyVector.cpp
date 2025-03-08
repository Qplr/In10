#include "EasyVector.h"

EasyVector::operator long long() const
{
	return (x + y) * (x + y + 1) / 2 + y;
}

EasyVector::operator sf::Vector2f() const
{
	return sf::Vector2f(x, y);
}

EasyVector operator-(const EasyVector& first, const EasyVector& second)
{
	return EasyVector(first - second);
}
