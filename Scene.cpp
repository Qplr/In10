#include "Scene.h"
#include "SFML/Graphics.hpp"
#include <format>

#include <time.h>

//	debug
#include <iostream>
//	\debug

bool Scene::tileEmpty(cvr pos) const
{
	if (gates.contains(pos))
		return false;
	if (crosses.contains(pos))
		return false;
	for (const auto& wg : wireGroups)
		if (wg.wireTiles.contains(pos))
			return false;
	return true;
}

void Scene::deleteWireGroup(int index)
{
	for (auto& gate : gates)
		gate.second.unLinkWire(&wireGroups[index]);
	wireGroups.erase(wireGroups.begin() + index);
}

std::pair<Gate*, bool> Scene::doesNewWireConnectToGate(cvr pos, Tile::Side side, Tile::Type type)
{
	auto tempPos = Tile::neighbour(pos, side);
	bool crossesInBetween = false;

	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
	{
		tempPos = Tile::neighbour(tempPos, side);
		crossesInBetween = true;
	}
	if (gates.contains(tempPos))
		return std::make_pair(& gates[tempPos], crossesInBetween);
	return std::make_pair(nullptr, false);
}

bool Scene::connectsToWire(const WireGroup& wg, cvr pos, Tile::Side side)
{
	auto tempPos = Tile::neighbour(pos, side);

	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
		tempPos = Tile::neighbour(tempPos, side);

	if (wg.wireTiles.contains(tempPos)) // if the tile to the specified side exists
		return true;

	return false;
}

bool Scene::doesNewWireConnectToWire(const WireGroup& wg, cvr pos)
{
	for (auto side : Tile::directions)
		if (connectsToWire(wg, pos, side))
			return true;
	return false;
}

Tile::Type Scene::doesNewGateConnectToWire(const WireGroup& wg, cvr pos, Tile::Side side)
{
	auto tempPos = Tile::neighbour(pos, side);

	// first, check if gate immediately connects to an input
	if (wg.wireTiles.contains(tempPos)) // if the tile to the specified side exists
		if (wg.wireTiles.at(tempPos) == Tile::INPUT)
			return Tile::INPUT; // wire group is an input for gate
	// then check for connection to outputs
	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
		tempPos = Tile::neighbour(tempPos, side);

	if (wg.wireTiles.contains(tempPos)) // if the tile to the specified side exists
		return Tile::WIRE;
	return Tile::VOID;
}

v Scene::ptc(cvr pixels) const
{
	return pixels / squareSize;
}

v Scene::ctp(cvr coords) const
{
	return sf::Vector2i(coords) * squareSize;
}

Scene::Scene()
{
	{
		window.create(sf::VideoMode(800, 800), "");
	}
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
}

void Scene::placeWire(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;
	// make sure new wire is in a group
	bool groupFound = false;
	int resultWireGroup = -1; // all groups that need merging will be merged to this one
	for(int i = 0; i < wireGroups.size(); i++)
		if (doesNewWireConnectToWire(wireGroups[i], pos)) // if point can be added to group
		{
			if (resultWireGroup != -1) // if we need to merge 2 groups
			{
				wireGroups[resultWireGroup].merge(std::move(wireGroups[i]));
				deleteWireGroup(i);
				i--;
			}
			else
			{
				groupFound = true;
				// can be added (but only once) straight away since we add a wire that belongs there
				wireGroups[i].wireTiles.emplace(pos, type);
				resultWireGroup = i;
			}
		}
	if (!groupFound) // if point was not connected to any group - create new group
	{
		resultWireGroup = wireGroups.size();
		wireGroups.emplace_back(WireGroup(crosses, gates));
		wireGroups.back().wireTiles.emplace(pos, type);
	}
	// connections to gates may occur
	for (auto side : Tile::directions)
	{
		if (auto gateAndMode = doesNewWireConnectToGate(pos, side, type); gateAndMode.first != nullptr)
		{
			WireGroup* wireGroup = &wireGroups[resultWireGroup];
			if (type == Tile::WIRE || gateAndMode.second)
				// if GATE is connected to INPUT, but right next to the gate CROSS is found - input is considered an output
				gateAndMode.first->linkWire(wireGroup);
			else if(type == Tile::INPUT)
				wireGroup->linkGate(gateAndMode.first);
		}
	}
}

void Scene::placeGate(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;

	gates.emplace(pos, Gate(type));
	// connections to wires may occur
	for (auto side : Tile::directions)
	{
		for(auto& wg: wireGroups)
			if (auto wireType = doesNewGateConnectToWire(wg, pos, side); wireType != Tile::VOID)
			{
				WireGroup* wireGroup = &wg;
				Gate* gate = &gates[pos];
				switch (wireType)
				{
				case Tile::WIRE:
					gate->linkWire(wireGroup);
					break;
				case Tile::INPUT:
					wireGroup->linkGate(gate);
					break;
				}
			}
	}
}

void Scene::placeCross(cvr pos)
{
	if (!tileEmpty(pos))
		return;

	crosses.insert(pos);
}

void Scene::placeSwitch(cvr pos)
{

}

void Scene::print()
{
	const float scale = 1;// float(squareSize) / textureSize;
	window.clear();
	sf::RectangleShape r(sf::Vector2f(32, 32));

	auto stateColor = [this](bool state)
		{
			return state ? sf::Color::White : sf::Color(64, 64, 64);
		};
	// select panel
	for (int i = 0; i < textures.size(); i++)
	{
		r.setTexture(&textures[i]);
		r.setPosition(0, i * 36);
		r.setFillColor(stateColor(selectedTile == Tile::tiles[i]));
		window.draw(r);
	}
	// wires
	r.setSize(v(squareSize, squareSize));
	r.setScale(scale, scale);
	for (auto& wg : wireGroups)
	{
		r.setFillColor(stateColor(wg.outputs.size() > 0)); // wg.state
		for (auto& wireTile : wg.wireTiles)
		{
			r.setPosition(ctp(wireTile.first));
			r.setTexture(&textures[static_cast<int>(wireTile.second)]);
			window.draw(r);
		}
	}
	// gates
	for (auto& gate : gates)
	{
		r.setPosition(ctp(gate.first));
		r.setFillColor(stateColor(gate.second.outputs.size() > 0)); // gate.second.state
		r.setTexture(&textures[static_cast<int>(gate.second.type)]);
		window.draw(r);
	}
	// crosses
	r.setFillColor(sf::Color::White);
	for (auto& cross : crosses)
	{
		r.setPosition(ctp(cross));
		r.setTexture(&textures[Tile::CROSS]);
		window.draw(r);
	}
	
	window.display();
}

void Scene::eventLoop()
{
	sf::Event e;
	clock_t lastFrame = clock();

	window.setFramerateLimit(fps);
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
				if(e.key.code == sf::Mouse::Right)
					switch (Tile::tiles[selectedTile])
					{
					case Tile::OR:
					case Tile::NOR:
					case Tile::AND:
					case Tile::NAND:
					case Tile::XOR:
					case Tile::XNOR:
						placeGate(ptc(sf::Mouse::getPosition(window)), Tile::tiles[selectedTile]);
						break;
					case Tile::WIRE:
					case Tile::INPUT:
						placeWire(ptc(sf::Mouse::getPosition(window)), Tile::tiles[selectedTile]);
						break;
					case Tile::CROSS:
						placeCross(ptc(sf::Mouse::getPosition(window)));
						break;
					case Tile::SWITCH:
						placeSwitch(ptc(sf::Mouse::getPosition(window)));
						break;
					default: abort();
					}
			}
		}
		if(clock() > lastFrame + CLOCKS_PER_SEC / fps)
		{
			lastFrame = clock();
			print();
		}
	}
}
