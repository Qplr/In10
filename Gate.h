#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"
#include "Tile.h"
#include "Orientation.h"

class WireGroup;

class Gate : public Tile
{
	v _pos;
	Orientation _orientation;
public:
	Gate(const Gate& another) = delete;
	Gate(v pos, Type type);

	v pos() const { return _pos; }
	virtual Orientation& orientation(cvr pos = EasyVector()) override { return _orientation; };
	virtual bool contains(cvr pos) const { return pos == _pos; }

	friend class Scene;
	friend class WireGroup;
};

