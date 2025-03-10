#pragma once

#include "WireGroup.h"
#include "Gate.h"
#include "defines.h"

#include <vector>
#include <set>

class Scene
{
	std::vector<WireGroup*> _wireGroups;
	std::vector<Gate*> _gates;
	std::set<v> _crosses;

	void deleteWireGroup(int index);
	void deleteGate(int index);

	int _selectedTileId = 0;
	Tile::Type _selectedTile = Tile::tiles[_selectedTileId];

	int tps = 10;
	std::set<Tile*> updatedGates;
	std::set<Tile*> updatedWires;
	clock_t lastTick = clock();

	Tile::Type tileType(cvr pos)const;
	std::pair<v, Tile*> sideConnectsToSpecificTileOf(v pos, Tile::Side side, const std::vector<Tile*>& wg);
	std::pair<v, int> sideConnectsToWire(v pos, Tile::Side side);
	std::pair<v, int> sideConnectsToGate(v pos, Tile::Side side);
	int wireAt(cvr pos);
	int gateAt(cvr pos);
	void connectIfPossible(WireGroup* wg, v pos, Tile::Side direction);
	WireGroup* isolateIfPossible(WireGroup* wg, v pos, Tile::Side side);

	void placeWire(cvr pos, Tile::Type type);
	void placeGate(cvr pos, Tile::Type type);
	void placeCross(cvr pos);

	void removeWire(cvr pos);
	void removeGate(cvr pos);
	void removeCross(cvr pos);

#ifdef _DEBUG
	void debug()const;
#endif
public:
	Scene();
	~Scene();

	const std::set<v>& crosses() const { return _crosses; }
	const std::vector<WireGroup*> wireGroups() const { return _wireGroups; }
	const std::vector<Gate*> gates() const { return _gates; }

	void leftClick(cvr pos);
	void rightClick(cvr pos);
	void scrollSelectedTile(int delta) { _selectedTileId = (_selectedTileId - delta + Tile::tiles.size()) % Tile::tiles.size(); _selectedTile = Tile::tiles[_selectedTileId]; }
	Tile::Type selectedTile() const { return _selectedTile; }
	
	void tick();
};

