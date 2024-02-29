#pragma once

#include "SFML/Graphics.hpp"

#include <map>
#include <unordered_map>
#include <set>

#include "defines.h"
#include "Tile.h"

class Gate;

class WireGroup: public Tile
{
	std::map<v, Type> wireTiles;
	std::set<v>& crosses;
	bool state = false;
	std::vector<Gate*> outputs, inputs;
public:
	WireGroup(std::set<v>& crosses, std::map<v, Gate>& gates) : crosses(crosses) {}
	WireGroup(const WireGroup&& another);
	WireGroup(const WireGroup& another);
	void linkGate(Gate* gate);
	void unLinkGate(Gate* gate);
	void unlinkAll();
	void merge(WireGroup&& another);
	WireGroup& operator=(const WireGroup& another);
	WireGroup& operator=(const WireGroup&& another);

	friend class Scene;
	friend class Gate;
};

