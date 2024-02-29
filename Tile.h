#pragma once

#include "defines.h"
#include "vector"

class Tile
{
public:
	enum Type
	{
		OR = 0, NOR, AND, NAND, XOR, XNOR,
		WIRE, INPUT, CROSS,
		SWITCH,
		VOID
	};
	enum Side
	{
		N, S, W, E
	};

	static const std::vector<Tile::Type> tiles;
	static const std::vector<Tile::Side> directions;
	static const std::map<Tile::Type, const char*> tileStrName;

	static cvr neighbour(cvr pos, Side side);
};

