#include "Tile.h"

void Tile::addOutput(Tile* wg)
{
	if (std::find(_outputs.begin(), _outputs.end(), wg) == _outputs.end())
		_outputs.push_back(wg);
}

void Tile::addInput(Tile* wg)
{
	if (std::find(_inputs.begin(), _inputs.end(), wg) == _inputs.end())
		_inputs.push_back(wg);
}

void Tile::removeOutput(Tile* wg)
{
	if (auto it = std::find(_outputs.begin(), _outputs.end(), wg); it != _outputs.end())
		_outputs.erase(it);
}

void Tile::removeInput(Tile* wg)
{
	if (auto it = std::find(_inputs.begin(), _inputs.end(), wg); it != _inputs.end())
		_inputs.erase(it);
}

void Tile::unlink(Tile* wg)
{
	removeInput(wg);
	wg->removeOutput(this);
	removeOutput(wg);
	wg->removeInput(this);
}

void Tile::unlinkAll()
{
	for (auto input : _inputs)
		unlink(input);
	for (auto output : _outputs)
		unlink(output);
}

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

Tile::Side Tile::reverse(Side side)
{
	switch (side)
	{
	case N: return S;
	case S: return N;
	case W: return E;
	case E: return W;
	default: return side;
	}
}
