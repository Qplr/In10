#pragma once

#include "defines.h"
#include "vector"

class Orientation;
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
protected:
	Type _type = VOID;
	bool _state = false;
	std::vector<Tile*> _outputs, _inputs;
public:
	Tile(Type type) :_type(type) {}
	Type type() const { return _type; }
	bool state() const { return _state; }
	bool setState(bool newState) { std::swap(_state, newState); return _state != newState; }
	const std::vector<Tile*>& outputs() const { return _outputs; }
	const std::vector<Tile*>& inputs() const { return _inputs; }
	void addOutput(Tile* wg);
	void addInput(Tile* wg);
	void removeOutput(Tile* wg);
	void removeInput(Tile* wg);

	void unlink(Tile* wg);
	void unlinkAll();

	virtual bool contains(cvr pos) const = 0; // for gate - compares to its pos, for wiregroup - calls contains on wireTiles
	virtual Orientation& orientation(cvr pos = EasyVector()) = 0; // for gate - ignored, for wiregroup - returns tileOrinetations


	static const std::vector<Tile::Type> tiles;
	static const std::vector<Tile::Side> directions;
	static const std::map<Tile::Type, const char*> tileStrName;

	static cvr neighbour(cvr pos, Side side);
	static Side reverse(Side side);
};

