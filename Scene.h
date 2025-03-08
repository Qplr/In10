#pragma once

#include "WireGroup.h"
#include "Gate.h"
#include "defines.h"

#include <vector>
#include <set>

class Scene
{
	const int viewport = 800, fps = 60, tps = 5, textureSize = 32;
	int squareSize = 25;
	sf::RenderWindow window;
	std::vector<sf::Texture> textures;

	std::vector<WireGroup*> wireGroups;
	std::vector<Gate*> gates;
	std::set<v> crosses;
	int selectedTile = 0;

	std::set<Tile*> updatedGates;
	std::set<Tile*> updatedWires;

	bool tileEmpty(cvr pos)const;
	std::pair<v, int> sideConnectsToWire(v pos, Tile::Side side);
	std::pair<v, int> sideConnectsToGate(v pos, Tile::Side side);
	int wireAt(cvr pos);
	int gateAt(cvr pos);
	void connectIfPossible(WireGroup* wg, v pos, Tile::Side direction);

	v ptc(cvr pixels)const;
	v ctp(cvr coords)const;

	void debug()const;
public:
	Scene();
	~Scene();

	void leftClick(cvr pos);
	void rightClick(cvr pos);
	void placeWire(cvr pos, Tile::Type type);
	void placeGate(cvr pos, Tile::Type type);
	void placeCross(cvr pos);

	void print();
	void tick();
	void eventLoop();
};

