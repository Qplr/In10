#include "Scene.h"
#include "SFML/Graphics.hpp"
#include <numeric>
#include <time.h>
#include <stack>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

void Scene::deleteWireGroup(int index)
{
	delete _wireGroups[index];
	_wireGroups.erase(_wireGroups.begin() + index);
}

void Scene::deleteGate(int index)
{
	delete _gates[index];
	_gates.erase(_gates.begin() + index);
}

Tile::Type Scene::tileType(cvr pos) const
{
	if (_crosses.contains(pos))
		return Tile::CROSS;
	for (auto gate : _gates)
		if (gate->contains(pos))
			return gate->type();
	for (auto wg : _wireGroups)
		if (wg->wireTiles().contains(pos))
			return wg->wireTiles()[pos];
	return Tile::VOID;
}

std::pair<v, Tile*> Scene::sideConnectsToSpecificTileOf(v pos, Tile::Side side, const std::vector<Tile*>& g)
{
	do
		pos = Tile::neighbour(pos, side);
	while (_crosses.contains(pos));
	for (int i = 0; i < g.size(); i++)
		if (g[i]->contains(pos))
			return std::make_pair(pos, g[i]);
	return { pos, nullptr };
}

std::pair<v, int> Scene::sideConnectsToWire(v pos, Tile::Side side)
{
	do
		pos = Tile::neighbour(pos, side);
	while (_crosses.contains(pos));
	return std::make_pair(pos, wireAt(pos));
}

std::pair<v, int> Scene::sideConnectsToGate(v pos, Tile::Side side)
{
	do
		pos = Tile::neighbour(pos, side);
	while (_crosses.contains(pos));
	return std::make_pair(pos, gateAt(pos));
}

int Scene::wireAt(cvr pos)
{
	for (int i = 0; i < _wireGroups.size(); i++)
		if (_wireGroups[i]->contains(pos))
			return i;
	return -1;
}

int Scene::gateAt(cvr pos)
{
	for (int i = 0; i < _gates.size(); i++)
		if(_gates[i]->pos() == pos)
			return i;
	return -1;
}

void Scene::connectIfPossible(WireGroup* wg, v pos, Tile::Side direction)
{
	// looks for a tile to the side (while skipping consecutive CROSSes) and connects accordingly
	if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1) // look for wire
	{
		v wirePosition = wirePosAndIndex.first;
		int wireGroupIndex = wirePosAndIndex.second;

		if (_wireGroups[wireGroupIndex] != wg)
		{
			wg->merge(std::move(*_wireGroups[wireGroupIndex])); // merge 2 groups
			deleteWireGroup(wireGroupIndex);
		}

		wg->tileOrientations()[pos].setConnection(direction);
		wg->tileOrientations()[wirePosition].setConnection(Tile::reverse(direction));
	}
	else if (auto gatePosAndIndex = sideConnectsToGate(pos, direction); gatePosAndIndex.second != -1) // look for _gates
	{
		int gateIndex = gatePosAndIndex.second;
		if (wg->wireTiles()[pos] == Tile::WIRE || _gates[gateIndex]->type() == Tile::SWITCH) // condition for gate as source
		{
			_gates[gateIndex]->addOutput(wg);
			wg->addInput(_gates[gateIndex]);

			wg->tileOrientations()[pos].setInputFrom(direction);
			_gates[gateIndex]->orientation().setOutputTo(Tile::reverse(direction));
		}
		else if (wg->wireTiles()[pos] == Tile::INPUT) // other _gates as receivers if type is INPUT
		{
			_gates[gateIndex]->addInput(wg);
			wg->addOutput(_gates[gateIndex]);

			wg->tileOrientations()[pos].setOutputTo(direction);
			_gates[gateIndex]->orientation().setInputFrom(Tile::reverse(direction));
		}
	}
}

WireGroup* Scene::isolateIfPossible(WireGroup* wg, v pos, Tile::Side side)
{
	std::vector<std::pair<v, Tile::Type>> isolatedTiles; // only affected tiles. their orientations will remain in wg.tileOrientations
	std::vector<int> isolatedInputs; // _gates index
	std::vector<int> isolatedOutputs; // _gates index
	std::stack<std::pair<v, Tile::Side>> toStepTiles; // tile to visit and an origin direction

	if (auto wirePosAndIndex = sideConnectsToWire(pos, side); wirePosAndIndex.second != -1)
	{
		v stepPos = wirePosAndIndex.first; // make a wire to this side a first step
		Tile::Side origin = Tile::reverse(side); // set origin
		
		toStepTiles.push(std::make_pair(stepPos, origin)); // first step

		// collect isolated tiles
		while (!toStepTiles.empty())
		{
			stepPos = toStepTiles.top().first;
			origin = toStepTiles.top().second;
			toStepTiles.pop();

			isolatedTiles.push_back(std::make_pair(stepPos, wg->wireTiles()[stepPos])); //save tile
			wg->wireTiles().erase(stepPos); // delete from original wire group (also to protect from loops)

			// TODO pass <wg> to <sideConnectsToWire>, <outputs> and <inputs> to <sideConnectsToGate> to reduce search
			// as we dont really need their indexes to reconnect
			for (auto direction : Tile::directions)
			{
				if (direction == origin) // don't look back
					continue;
				bool inputFrom = wg->tileOrientations()[stepPos].hasInputFrom(direction); // do we have input from direction
				bool outputTo = wg->tileOrientations()[stepPos].hasOutputTo(direction); // do we have an output to direction
				if (inputFrom && !outputTo) // only input from direction (from gate)
					isolatedInputs.push_back(sideConnectsToGate(stepPos, direction).second);
				else if (!inputFrom && outputTo) // only output to direction (to gate)
					isolatedOutputs.push_back(sideConnectsToGate(stepPos, direction).second);
				else if (inputFrom && outputTo) // both input and output (wire)
				{
					if (auto wirePosAndIndex = sideConnectsToWire(stepPos, direction); wirePosAndIndex.second != -1) // check if there is actually a wire or we did this step already
						toStepTiles.push(std::make_pair( // next step
							wirePosAndIndex.first,
							Tile::reverse(direction)));
				}
			}
		}
		// create new WireGroup
		auto newWireGroup = new WireGroup(isolatedTiles);
		for (auto pos : isolatedTiles)
			newWireGroup->tileOrientations().insert(std::make_pair(pos.first, wg->tileOrientations()[pos.first]));
		for (int input : isolatedInputs)
		{
			newWireGroup->addInput(_gates[input]);
			_gates[input]->addOutput(newWireGroup);
			_gates[input]->removeOutput(wg);
		}
		for (int output : isolatedOutputs)
		{
			newWireGroup->addOutput(_gates[output]);
			_gates[output]->addInput(newWireGroup);
			_gates[output]->removeInput(wg);
		}
		return newWireGroup;
	}
	else if (auto gatePosAndIndex = sideConnectsToGate(pos, side); gatePosAndIndex.second != -1)
	{
		Gate* gate = _gates[gatePosAndIndex.second];
		gate->unlink(wg);
		gate->orientation().unSetConnection(Tile::reverse(side));
	}
	return nullptr;
}

#ifdef _DEBUG
void Scene::debug() const
{
	static int counter = 0;
	counter++;
	std::cout << "debug() counter: " << counter << std::endl;
	for (auto g : _wireGroups)
	{
		std::cout << "group " << unsigned(g) << " i: " << std::endl;
		for (auto input : g->inputs())
			std::cout << unsigned(input) << std::endl;
		std::cout << " o: " << std::endl;
		for (auto output : g->outputs())
			std::cout << unsigned(output) << std::endl;
	}
	for (auto g : _gates)
	{
		std::cout << "gate " << unsigned(g) << " i: " << std::endl;
		for (auto input : g->inputs())
			std::cout << unsigned(input) << std::endl;
		std::cout << " o: " << std::endl;
		for (auto output : g->outputs())
			std::cout << unsigned(output) << std::endl;
	}
}
#endif

Scene::Scene()
{

}

Scene::~Scene()
{
	for (auto w : _wireGroups)
		delete w;
	for (auto g : _gates)
		delete g;
}

void Scene::leftClick(cvr pos)
{
	Tile::Type clickedTileType = tileType(pos);
	switch (clickedTileType)
	{
	case Tile::OR:
	case Tile::NOR:
	case Tile::AND:
	case Tile::NAND:
	case Tile::XOR:
	case Tile::XNOR:
	case Tile::SWITCH:
		removeGate(pos);
		break;
	case Tile::WIRE:
	case Tile::INPUT:
		removeWire(pos);
		break;
	case Tile::CROSS:
		removeCross(pos);
		break;
	default:
		break;
	}
}

void Scene::rightClick(cvr pos)
{
	if (int gateIndex = gateAt(pos); gateIndex != -1)
		if (_gates[gateIndex]->type() == Tile::SWITCH)
			updatedGates.insert(_gates[gateIndex]);
	switch (_selectedTile)
	{
	case Tile::OR:
	case Tile::NOR:
	case Tile::AND:
	case Tile::NAND:
	case Tile::XOR:
	case Tile::XNOR:
	case Tile::SWITCH:
		placeGate(pos, _selectedTile);
		break;
	case Tile::WIRE:
	case Tile::INPUT:
		placeWire(pos, _selectedTile);
		break;
	case Tile::CROSS:
		placeCross(pos);
		break;
	default: abort();
	}
}

void Scene::placeWire(cvr pos, Tile::Type type)
{
	if (tileType(pos) != Tile::VOID)
		return;


	auto newWireGroup = new WireGroup(pos, type); // new group with new wire
	_wireGroups.push_back(newWireGroup);
	
	for (auto direction : Tile::directions) // for every direction
		connectIfPossible(newWireGroup, pos, direction);

#ifdef _DEBUG
	debug();
#endif
}

void Scene::placeGate(cvr pos, Tile::Type type)
{
	if (tileType(pos) != Tile::VOID)
		return;

	auto newGate = new Gate(pos, type);
	for (auto direction : Tile::directions) // for every direction
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1) // look for wire
		{
			WireGroup* wireGroup = _wireGroups[wirePosAndIndex.second];
			v wirePosition = wirePosAndIndex.first;
			Tile::Type wireType = wireGroup->wireTiles()[wirePosition];
			if (wireType == Tile::WIRE || newGate->type() == Tile::SWITCH) // condition for gate as source
			{
				newGate->addOutput(wireGroup);
				wireGroup->addInput(newGate);

				wireGroup->orientation(wirePosition).setInputFrom(Tile::reverse(direction));
				newGate->orientation().setOutputTo(direction);
			}
			else
			{
				newGate->addInput(wireGroup);
				wireGroup->addOutput(newGate);

				wireGroup->orientation(wirePosition).setOutputTo(Tile::reverse(direction));
				newGate->orientation().setInputFrom(direction);
			}
		}
	_gates.push_back(newGate);

	if (type != Tile::SWITCH)
		updatedGates.insert(newGate);

#ifdef _DEBUG
	debug();
#endif
}

void Scene::placeCross(cvr pos)
{
	if (tileType(pos) != Tile::VOID)
		return;

	_crosses.insert(pos);
	
	for (auto direction : { Tile::N, Tile::S })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
			connectIfPossible(_wireGroups[wirePosAndIndex.second], wirePosAndIndex.first, Tile::reverse(direction));
	for (auto direction : { Tile::W, Tile::E })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
			connectIfPossible(_wireGroups[wirePosAndIndex.second], wirePosAndIndex.first, Tile::reverse(direction));

#ifdef _DEBUG
	debug();
#endif
}

void Scene::removeWire(cvr pos)
{
	int wgIndex = wireAt(pos);
	WireGroup* wg = _wireGroups[wgIndex];

	std::vector<WireGroup*> newWireGroups;

	// remove tile from group
	wg->wireTiles().erase(pos);
	wg->tileOrientations().erase(pos);

	// remove adjacent tiles orientations towards deleted tile
	// those tiles can only either be wires from same group or a gate from inputs or outputs
	for (auto direction : Tile::directions)
	{
		if (auto wirePosAndPtr = sideConnectsToSpecificTileOf(pos, direction, { wg }); wirePosAndPtr.second != nullptr)
			wg->tileOrientations()[wirePosAndPtr.first].unSetConnection(Tile::reverse(direction));
		else if (wg->wireTiles()[pos] == Tile::WIRE)
		{
			if (auto gatePosAndPtr = sideConnectsToSpecificTileOf(pos, direction, wg->inputs()); gatePosAndPtr.second != nullptr)
				gatePosAndPtr.second->orientation().unSetConnection(Tile::reverse(direction));
		}
		else if (wg->wireTiles()[pos] == Tile::INPUT)
		{
			if (auto gatePosAndPtr = sideConnectsToSpecificTileOf(pos, direction, wg->outputs()); gatePosAndPtr.second != nullptr)
				gatePosAndPtr.second->orientation().unSetConnection(Tile::reverse(direction));
		}
	}

	for (auto direction : Tile::directions)
	{
		if (auto newWg = isolateIfPossible(wg, pos, direction); newWg != nullptr)
			newWireGroups.push_back(newWg);
	}
	deleteWireGroup(wgIndex);
	for (auto newWg : newWireGroups)
		_wireGroups.push_back(newWg);

#ifdef _DEBUG
	debug();
#endif
}

void Scene::removeGate(cvr pos)
{
	int gateIndex = gateAt(pos);
	Gate* gate = _gates[gateIndex];

	for (auto direction : Tile::directions)
	{
		Tile* wg;
		v wirePos;
		if (gate->orientation().hasInputFrom(direction))
		{
			if (auto tilePosAndPtr = sideConnectsToSpecificTileOf(pos, direction, gate->inputs()); tilePosAndPtr.second != nullptr)
			{
				wirePos = tilePosAndPtr.first;
				wg = tilePosAndPtr.second;

				wg->removeOutput(gate);

				wg->orientation(wirePos).unSetConnection(Tile::reverse(direction));
			}
		}
		else if (gate->orientation().hasOutputTo(direction))
		{
			if (auto tilePosAndPtr = sideConnectsToSpecificTileOf(pos, direction, gate->outputs()); tilePosAndPtr.second != nullptr)
			{
				wirePos = tilePosAndPtr.first;
				wg = tilePosAndPtr.second;

				wg->removeInput(gate);

				wg->orientation(wirePos).unSetConnection(Tile::reverse(direction));
			}
		}
	}
	deleteGate(gateIndex);

#ifdef _DEBUG
	debug();
#endif
}

void Scene::removeCross(cvr pos)
{
	std::vector<WireGroup*> newWireGroups;
	
	int verticalIndex = -1;
	int horizontalIndex = -1;

	auto isolate = [this, &pos, &newWireGroups](std::vector<Tile::Side> dirs, int wgIndex)
		{
			if (wgIndex == -1)
				return;
			for (auto direction : dirs)
			{
				if (auto newWg = isolateIfPossible(_wireGroups[wgIndex], pos, direction); newWg != nullptr)
					newWireGroups.push_back(newWg);
			}
			deleteWireGroup(wgIndex);
		};

	for (auto direction : { Tile::N, Tile::S })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
		{
			isolate({ Tile::N, Tile::S }, wirePosAndIndex.second);
			break;
		}
	for (auto direction : { Tile::W, Tile::E })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
		{
			isolate({ Tile::W, Tile::E }, wirePosAndIndex.second);
			break;
		}

	for (auto newWg : newWireGroups)
		_wireGroups.push_back(newWg);

	_crosses.erase(pos);

#ifdef _DEBUG
	debug();
#endif
}

void Scene::tick()
{
	if (clock() > lastTick + CLOCKS_PER_SEC / tps)
		lastTick = clock();
	else
		return;

	// update _gates
	for (auto gate : updatedGates)
	{
		updatedWires.insert(gate->outputs().begin(), gate->outputs().end());
		switch (gate->type())
		{
		case Tile::SWITCH:
			gate->setState(!gate->state());
			break;
		case Tile::OR:
			gate->setState(std::accumulate(gate->inputs().begin(),gate->inputs().end(), false, [](bool result, Tile* second) {return result || second->state(); }));
			break;
		case Tile::NOR:
			gate->setState(std::accumulate(gate->inputs().begin(), gate->inputs().end(), true, [](bool result, Tile* second) {return result && !second->state(); }));
			break;
		case Tile::AND:
			gate->setState(std::accumulate(gate->inputs().begin(), gate->inputs().end(), true, [](bool result, Tile* second) {return result && second->state(); }));
			break;
		case Tile::NAND:
			gate->setState(std::accumulate(gate->inputs().begin(), gate->inputs().end(), false, [](bool result, Tile* second) {return result || !second->state(); }));
			break;
		case Tile::XOR:
			gate->setState(std::accumulate(gate->inputs().begin(), gate->inputs().end(), false, [](bool result, Tile* second) {return result ^ second->state(); }));
			break;
		case Tile::XNOR:
			gate->setState(
				std::accumulate(gate->inputs().begin(), gate->inputs().end(), true, [](bool result, Tile* second) {return result && second->state(); }) // AND
				||
				std::accumulate(gate->inputs().begin(), gate->inputs().end(), true, [](bool result, Tile* second) {return result && !second->state(); })); // NOR
			break;
		}
	}
	updatedGates.clear();
	// update wires
	for (auto wire : updatedWires)
	{
		updatedGates.insert(wire->outputs().begin(), wire->outputs().end());
		wire->setState(std::accumulate(wire->inputs().begin(), wire->inputs().end(), false, [](bool result, Tile* second) {return result || second->state(); }));
	}
	updatedWires.clear();
}
