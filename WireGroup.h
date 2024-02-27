#pragma once

#include "SFML/Graphics.hpp"

#include <map>
#include <unordered_map>
#include <set>

#include "defines.h"

class Gate;

class WireGroup
{
public:
	enum Side
	{
		N, S, W, E
	};
	enum Type
	{
		WIRE, INPUT
	};
private:
	std::map<v, Type> wireTiles;
	std::set<v>& crosses;
	bool state = false;
	std::map<v, Gate*> inputs;
	bool checkSide(cvr pos, Type type, Side side);
public:
	WireGroup(std::set<v>& crosses) : crosses(crosses) {}
	WireGroup(const WireGroup&& another);
	WireGroup(const WireGroup& another);
	WireGroup* doesPointConnect(cvr pos, Type type);
	void merge(WireGroup&& another);
	static cvr neighbour(cvr pos, Side side);
	WireGroup& operator=(const WireGroup& another);

	friend class Scene;
};

