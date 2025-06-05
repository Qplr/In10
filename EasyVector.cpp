#include "EasyVector.h"

bool operator<(const EasyVector& lhs, const EasyVector& rhs)
{
	return lhs.x == rhs.x ? lhs.y < rhs.y :  lhs.x < rhs.x;
}

EasyVector::operator sf::Vector2f()
{
	return sf::Vector2f(x, y);
}
