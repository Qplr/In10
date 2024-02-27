#include "WireGroup.h"

bool WireGroup::checkSide(cvr pos, Type type, Side side) // true - the object has joined new point and other neightbouring wire groups
{
	auto tempPos = neighbour(pos, side);

	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
		tempPos = neighbour(tempPos, side);

	if (wireTiles.contains(tempPos)) // if the tile to the specified side exists
		return true;
	return false;
}

WireGroup::WireGroup(const WireGroup&& another) : crosses(another.crosses)
{
	wireTiles = std::move(another.wireTiles);
	inputs = std::move(another.inputs);
}

WireGroup::WireGroup(const WireGroup& another) : crosses(another.crosses)
{
	*this = another;
}

WireGroup* WireGroup::doesPointConnect(cvr pos, Type type)
{
	if (checkSide(pos, type, N)) // if point was joined - end and return this for reference
		return this;
	if (checkSide(pos, type, S))
		return this;
	if (checkSide(pos, type, W))
		return this;
	if (checkSide(pos, type, E))
		return this;
	return nullptr;
}

void WireGroup::merge(WireGroup&& another)
{
	wireTiles.merge(another.wireTiles);
	inputs.merge(another.inputs);
	state = another.state;
}

cvr WireGroup::neighbour(cvr pos, Side side)
{
	switch (side)
	{
	case N: return v(pos.x, pos.y - 1);
	case S: return v(pos.x, pos.y + 1);
	case W: return v(pos.x - 1, pos.y);
	case E: return v(pos.x + 1, pos.y);
	default: return pos;
	}
}

WireGroup& WireGroup::operator=(const WireGroup& another)
{
	wireTiles = another.wireTiles;
	inputs = another.inputs;
	state = another.state;
	return *this;
}
