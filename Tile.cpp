#include "Tile.h"

const std::vector<Tile::Type> Tile::tiles = { OR, NOR, AND, NAND, XOR, XNOR, WIRE, INPUT, CROSS, SWITCH };
const std::vector<Tile::Side> Tile::directions = { N, S, W, E };
const std::map<Tile::Type, const char*> Tile::tileStrName =
{
	{OR, "or"},
	{NOR, "nor"},
	{AND, "and"},
	{NAND, "nand"},
	{XOR, "xor"},
	{XNOR, "xnor"},
	{WIRE, "wire"},
	{INPUT, "input"},
	{CROSS, "cross"},
	{SWITCH, "switch"}
};

cvr Tile::neighbour(cvr pos, Side side)
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
