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
	std::map<v, Gate>& gates;
	bool state = false;
	std::vector<Gate*> outputs;
	
public:
	WireGroup(std::set<v>& crosses, std::map<v, Gate>& gates) : crosses(crosses), gates(gates) {}
	WireGroup(const WireGroup&& another);
	WireGroup(const WireGroup& another);
	void linkGate(Gate* gate);
	void unLinkGate(Gate* gate);
	void merge(WireGroup&& another);
	WireGroup& operator=(const WireGroup& another);

	friend class Scene;
	friend class Gate;
};

