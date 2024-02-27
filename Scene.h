#pragma once

#include "WireGroup.h"
#include "Gate.h"
#include "defines.h"

#include <vector>
#include <set>

class Scene
{
	const int squareSize = 10, viewport = 800, fps = 60;
	sf::RenderWindow window;

	std::vector<WireGroup> wireGroups;
	std::map<v, Gate> gates;
	std::set<v> crosses;

	bool tileEmpty(cvr pos)const;
public:
	Scene();

	void placeWire(cvr pos, WireGroup::Type type);
	void placeGate(cvr pos, Gate::Type type);
	void placeCross(cvr pos);

	void print();
	void eventLoop();
};

