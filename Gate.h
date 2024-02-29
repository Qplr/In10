#pragma once

#include "SFML/Graphics.hpp"

#include "defines.h"
#include "Tile.h"

class WireGroup;

class Gate: public Tile
{
	bool state = false;
	Type type = VOID;
	std::vector<WireGroup*> outputs, inputs;
public:
	Gate() {}
	Gate(Type type);
	void linkWire(WireGroup* wg);
	void unLinkWire(WireGroup* wg);
	void unlinkAll();
	friend class Scene;
	friend class WireGroup;
};

