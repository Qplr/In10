#pragma once

#include "WireGroup.h"
#include "Gate.h"
#include "defines.h"

#include <vector>
#include <set>

class Scene
{
	const int viewport = 800, fps = 60, textureSize = 32;
	int squareSize = 5;
	sf::RenderWindow window;
	std::vector<sf::Texture> textures;

	std::vector<WireGroup> wireGroups;
	std::map<v, Gate> gates;
	std::set<v> crosses;
	int selectedTile = 0;

	bool tileEmpty(cvr pos)const;
	void deleteWireGroup(int index);
	//void deleteGate(Gate* gate);
	std::pair<Gate*, bool> doesNewWireConnectToGate(cvr pos, Tile::Side side);
	bool doesNewWireConnectToWire(cvr pos, const WireGroup& wg, Tile::Side side);
	Tile::Type doesNewGateConnectToWire(cvr pos, const WireGroup& wg, Tile::Side side);

	void wireToGate(cvr pos, Tile::Side side, Tile::Type type, WireGroup* wireGroup);
	int wireToWire(cvr pos, Tile::Type type, const std::vector<Tile::Side> directions);
	void gateToWire(cvr pos, Tile::Side side);
	v ptc(cvr pixels)const;
	v ctp(cvr coords)const;

	void debug()const;
public:
	Scene();
	~Scene();

	void placeWire(cvr pos, Tile::Type type);
	void placeGate(cvr pos, Tile::Type type);
	void placeCross(cvr pos);

	void print();
	void eventLoop();
};

