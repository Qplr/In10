#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"
#include "Tile.h"
#include "Orientation.h"

class WireGroup;

class Gate: public Tile
{
	v pos;
	Orientation orientation;
public:
	Gate(Gate&& another);
	Gate(const Gate& another) = delete;
	Gate(v pos, Type type);

	friend class Scene;
	friend class WireGroup;
};

