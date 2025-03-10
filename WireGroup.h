#pragma once

#include "SFML/Graphics.hpp"

#include <map>
#include <unordered_map>
#include <set>

#include "defines.h"
#include "Tile.h"
#include "Orientation.h"

class Gate;

class WireGroup: public Tile
{
	std::map<v, Tile::Type> _wireTiles;
	std::map<v, Orientation> _tileOrientations;
public:
	WireGroup(std::vector<std::pair<v, Tile::Type>> pos);
	WireGroup(v pos, Tile::Type type);
	WireGroup(const WireGroup& another) = delete;

	std::map<v, Tile::Type>& wireTiles() { return _wireTiles; }
	std::map<v, Orientation>& tileOrientations() { return _tileOrientations; }

	void merge(WireGroup&& another);
	virtual bool contains(cvr pos) const override { return _wireTiles.contains(pos); }
	virtual Orientation& orientation(cvr pos = EasyVector()) { return _tileOrientations[pos]; }
};

