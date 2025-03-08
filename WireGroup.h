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
	std::map<v, Tile::Type> wireTiles;
	std::map<v, Orientation> tileOrientations;
	v min, max;
public:
	WireGroup(v pos, Tile::Type type);
	WireGroup(WireGroup&& another);
	WireGroup(const WireGroup& another) = delete;

	void merge(WireGroup&& another);

	friend class Scene;
	friend class Gate;
};

