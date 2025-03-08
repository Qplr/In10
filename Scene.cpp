#include "Scene.h"
#include "SFML/Graphics.hpp"
#include <format>
#include <numeric>
#include <time.h>

//	debug
#include <iostream>
//	\debug

bool Scene::tileEmpty(cvr pos) const
{
	if (std::find_if(gates.begin(), gates.end(), [pos](Gate* gate) { return gate->pos == pos; }) != gates.end())
		return false;
	if (crosses.contains(pos))
		return false;
	for (auto wg : wireGroups)
		if (wg->wireTiles.contains(pos))
			return false;
	return true;
}

std::pair<v, int> Scene::sideConnectsToWire(v pos, Tile::Side side)
{
	do
		pos = Tile::neighbour(pos, side);
	while (crosses.contains(pos));
	return std::make_pair(pos, wireAt(pos));
}

std::pair<v, int> Scene::sideConnectsToGate(v pos, Tile::Side side)
{
	do
		pos = Tile::neighbour(pos, side);
	while (crosses.contains(pos));
	return std::make_pair(pos, gateAt(pos));
}

int Scene::wireAt(cvr pos)
{
	for (int i = 0; i < wireGroups.size(); i++)
		if (wireGroups[i]->wireTiles.contains(pos))
			return i;
	return -1;
}

int Scene::gateAt(cvr pos)
{
	for (int i = 0; i < gates.size(); i++)
		if(gates[i]->pos == pos)
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

		if (wireGroups[wireGroupIndex] != wg)
		{
			wg->merge(std::move(*wireGroups[wireGroupIndex])); // merge 2 groups
			delete wireGroups[wireGroupIndex]; // free WireGroup object
			wireGroups.erase(wireGroups.begin() + wireGroupIndex); // remove merged from list
		}

		wg->tileOrientations[pos].setConnection(direction);
		wg->tileOrientations[wirePosition].setConnection(Tile::reverse(direction));
	}
	else if (auto gatePosAndIndex = sideConnectsToGate(pos, direction); gatePosAndIndex.second != -1) // look for gates
	{
		int gateIndex = gatePosAndIndex.second;
		if (wg->wireTiles[pos] == Tile::WIRE || gates[gateIndex]->type() == Tile::SWITCH) // condition for gate as source
		{
			gates[gateIndex]->addOutput(wg);
			wg->addInput(gates[gateIndex]);

			wg->tileOrientations[pos].setInputFrom(direction);
			gates[gateIndex]->orientation.setOutputTo(Tile::reverse(direction));
		}
		else if (wg->wireTiles[pos] == Tile::INPUT) // other gates as receivers if type is INPUT
		{
			gates[gateIndex]->addInput(wg);
			wg->addOutput(gates[gateIndex]);

			wg->tileOrientations[pos].setOutputTo(direction);
			gates[gateIndex]->orientation.setInputFrom(Tile::reverse(direction));
		}
	}
}

v Scene::ptc(cvr pixels) const
{
	return pixels / squareSize;
}

v Scene::ctp(cvr coords) const
{
	return sf::Vector2i(coords) * squareSize;
}

void Scene::debug() const
{
	system("cls");
	for (auto wg : wireGroups)
		std::cout << "group " << unsigned int(wg) << " i: " << wg->inputs().size() << " o: " << wg->outputs().size() << std::endl;
	for (auto g : gates)
		std::cout << "gate " << unsigned int(g) << " i: " << g->inputs().size() << " o: " << g->outputs().size() << std::endl;
}

Scene::Scene()
{
	window.create(sf::VideoMode(800, 800), "");
	window.setFramerateLimit(fps);
	window.setVerticalSyncEnabled(true);

	textures.assign(Tile::tiles.size(), sf::Texture());
	for (int i = 0; i < Tile::tiles.size(); i++)
	{
		if (!textures[i].loadFromFile(std::format("resources/{}.bmp", Tile::tileStrName.at(Tile::tiles.at(i)))))
		{
			// texture missng
			// sf::Text t(std::format("missing texture: resources/{}.bmp", Tile::tileStrName.at(Tile::tiles.at(i))), sf::Font());
			abort();
		}
		else if (auto size = textures[i].getSize(); size.x != textureSize || size.y != textureSize)
		{
			// incorrect texture size
			abort();
		}
	}
}

Scene::~Scene()
{
	for (auto w : wireGroups)
		delete w;
	for (auto g : gates)
		delete g;
}

void Scene::leftClick(cvr pos)
{
}

void Scene::rightClick(cvr pos)
{
	if (int gateIndex = gateAt(pos); gateIndex != -1)
		if (gates[gateIndex]->type() == Tile::SWITCH)
			updatedGates.insert(gates[gateIndex]);
	switch (Tile::tiles[selectedTile])
	{
	case Tile::OR:
	case Tile::NOR:
	case Tile::AND:
	case Tile::NAND:
	case Tile::XOR:
	case Tile::XNOR:
	case Tile::SWITCH:
		placeGate(pos, Tile::tiles[selectedTile]);
		break;
	case Tile::WIRE:
	case Tile::INPUT:
		placeWire(pos, Tile::tiles[selectedTile]);
		break;
	case Tile::CROSS:
		placeCross(pos);
		break;
	default: abort();
	}
}

void Scene::placeWire(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;


	auto newWireGroup = new WireGroup(pos, type); // new group with new wire
	wireGroups.push_back(newWireGroup);
	
	for (auto direction : Tile::directions) // for every direction
		connectIfPossible(newWireGroup, pos, direction);

	debug();
}

void Scene::placeGate(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;

	auto newGate = new Gate(pos, type);
	for (auto direction : Tile::directions) // for every direction
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1) // look for wire
		{
			WireGroup* wireGroup = wireGroups[wirePosAndIndex.second];
			v wirePosition = wirePosAndIndex.first;
			Tile::Type wireType = wireGroup->wireTiles[wirePosition];
			if (wireType == Tile::WIRE || newGate->type() == Tile::SWITCH) // condition for gate as source
			{
				newGate->addOutput(wireGroup);
				wireGroup->addInput(newGate);

				wireGroup->tileOrientations[wirePosition].setInputFrom(Tile::reverse(direction));
				newGate->orientation.setOutputTo(direction);
			}
			else
			{
				newGate->addInput(wireGroup);
				wireGroup->addOutput(newGate);

				wireGroup->tileOrientations[wirePosition].setOutputTo(Tile::reverse(direction));
				newGate->orientation.setInputFrom(direction);
			}
		}
	gates.push_back(newGate);

	if (type != Tile::SWITCH)
		updatedGates.insert(newGate);

	debug();
}

void Scene::placeCross(cvr pos)
{
	if (!tileEmpty(pos))
		return;

	crosses.insert(pos);
	
	for (auto direction : { Tile::N, Tile::S })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
			connectIfPossible(wireGroups[wirePosAndIndex.second], wirePosAndIndex.first, Tile::reverse(direction));
	for (auto direction : { Tile::W, Tile::E })
		if (auto wirePosAndIndex = sideConnectsToWire(pos, direction); wirePosAndIndex.second != -1)
			connectIfPossible(wireGroups[wirePosAndIndex.second], wirePosAndIndex.first, Tile::reverse(direction));

	debug();
}

void Scene::print()
{
	window.clear();
	sf::RectangleShape r(sf::Vector2f(squareSize - 1, squareSize - 1));
	sf::RectangleShape ro(sf::Vector2f(4, 4));
	ro.setOrigin(-squareSize / 2 + 2, -squareSize / 2 + 2);

	auto stateColor = [](int state)
		{
			return state ? sf::Color::White : sf::Color(64, 64, 64);
		};
	auto visible = [this](cvr pos)
	{
			return pos.x >= -squareSize && pos.y >= -squareSize && pos.x < viewport && pos.y < viewport;
		};
	// wires
	for (auto wg : wireGroups)
	{
		r.setFillColor(stateColor(wg->state()));
		for (auto& wireTile : wg->wireTiles)
		{
			if (visible(ctp(wireTile.first)))
			{
			r.setPosition(ctp(wireTile.first));
			r.setTexture(&textures[static_cast<int>(wireTile.second)]);
			window.draw(r);
			}
		}
		for (auto& orientation : wg->tileOrientations)
		{
			auto pixelcoords = ctp(orientation.first);
			if (orientation.second.hasConnection(Tile::N))
			{
				ro.setPosition(pixelcoords.x, pixelcoords.y - 8);
				window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::S))
			{
				ro.setPosition(pixelcoords.x, pixelcoords.y + 8);
				window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::W))
			{
				ro.setPosition(pixelcoords.x - 8, pixelcoords.y);
				window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::E))
			{
				ro.setPosition(pixelcoords.x + 8, pixelcoords.y);
				window.draw(ro);
			}
		}
	}
	// gates
	for (auto gate : gates)
	{
		if (visible(ctp(gate->pos)))
		{
			r.setPosition(ctp(gate->pos));
			r.setFillColor(stateColor(gate->state()));
			r.setTexture(&textures[static_cast<int>(gate->type())]);
			window.draw(r);
		}

		auto pixelcoords = ctp(gate->pos);
		if (gate->orientation.hasConnection(Tile::N))
		{
			ro.setPosition(pixelcoords.x, pixelcoords.y - 8);
			window.draw(ro);
		}
		if (gate->orientation.hasConnection(Tile::S))
		{
			ro.setPosition(pixelcoords.x, pixelcoords.y + 8);
			window.draw(ro);
		}
		if (gate->orientation.hasConnection(Tile::W))
		{
			ro.setPosition(pixelcoords.x - 8, pixelcoords.y);
			window.draw(ro);
		}
		if (gate->orientation.hasConnection(Tile::E))
		{
			ro.setPosition(pixelcoords.x + 8, pixelcoords.y);
			window.draw(ro);
		}
	}
	// crosses
	r.setFillColor(sf::Color::White);
	for (auto cross : crosses)
	{
		if (visible(ctp(cross)))
		{
			r.setPosition(ctp(cross));
			r.setTexture(&textures[Tile::CROSS]);
			window.draw(r);
		}
	}
	// the selection panel
	r.setSize(v(32, 32));
	r.setOutlineThickness(1);
	for (int i = 0; i < textures.size(); i++)
	{
		r.setTexture(&textures[i]);
		r.setPosition(4, 4 + i * 36);
		r.setFillColor(stateColor(selectedTile == Tile::tiles[i]));
		window.draw(r);
	}
	
	window.display();
}

void Scene::tick()
{
	// update gates
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
	for (auto wire : updatedWires)
	{
		updatedGates.insert(wire->outputs().begin(), wire->outputs().end());
		wire->setState(std::accumulate(wire->inputs().begin(), wire->inputs().end(), false, [](bool result, Tile* second) {return result || second->state(); }));
	}
	updatedWires.clear();
}

void Scene::eventLoop()
{
	sf::Event e;
	clock_t lastFrame = clock();
	clock_t lastTick = clock();

	while (window.isOpen())
	{
		while (window.pollEvent(e))
		{
			if (e.type == sf::Event::Closed)
				window.close();
			if (e.type == sf::Event::MouseWheelMoved)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					squareSize += e.mouseWheel.delta;
				else
					selectedTile = (selectedTile - e.mouseWheel.delta + Tile::tiles.size()) % Tile::tiles.size();
			}
			if (e.type == sf::Event::MouseButtonPressed)
			{
				if (e.key.code == sf::Mouse::Right)
				{
					rightClick(ptc(sf::Mouse::getPosition(window)));
				}
			}
		}
		if(clock() > lastFrame + CLOCKS_PER_SEC / fps)
		{
			lastFrame = clock();
			print();
		}
		if (clock() > lastTick + CLOCKS_PER_SEC / tps)
		{
			lastTick = clock();
			tick();
		}
	}
}
