#include "EasyVector.h"

EasyVector::operator long long() const
{
	return (x + y) * (x + y + 1) / 2 + y;
}
