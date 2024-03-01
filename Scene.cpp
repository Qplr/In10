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
	wireGroups[index].unlinkAll();
	wireGroups.erase(wireGroups.begin() + index);
}

std::pair<Gate*, bool> Scene::doesNewWireConnectToGate(cvr pos, Tile::Side side)
{
	// true or false in return value describes if connection is made through CROSS tiles
	auto tempPos = Tile::neighbour(pos, side);

	// first, check if input immediately connects to a gate
	if (gates.contains(tempPos))
		return std::make_pair(&gates[tempPos], true); // return immediate connection

	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
		tempPos = Tile::neighbour(tempPos, side);
	if (gates.contains(tempPos))
		return std::make_pair(& gates[tempPos], false);
	return std::make_pair(nullptr, false);
}

bool Scene::doesNewWireConnectToWire(cvr pos, const WireGroup& wg,  Tile::Side side)
{
	auto tempPos = Tile::neighbour(pos, side);

	while (crosses.contains(tempPos)) //skipping all crosses in specified direction
		tempPos = Tile::neighbour(tempPos, side);

	if (wg.wireTiles.contains(tempPos)) // if the tile to the specified side exists
		return true;

	return false;
}

Tile::Type Scene::doesNewGateConnectToWire(cvr pos, const WireGroup& wg, Tile::Side side)
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

void Scene::wireToGate(cvr wirePos, Tile::Side side, Tile::Type type)
{
	WireGroup* wireGroup = nullptr;
	for (auto& wg : wireGroups)
		if (wg.wireTiles.contains(wirePos))
		{
			wireGroup = &wg;
			break;
		}
	if (wireGroup == nullptr)
		return;
	if (auto gateAndMode = doesNewWireConnectToGate(wirePos, side); gateAndMode.first != nullptr)
	{
		// SWITCH has no inputs and outputs to all WIRE variations
		// INPUT is considered a WIRE (output) for a gate if is separated by a CROSS
		if (type == Tile::INPUT && gateAndMode.second && gateAndMode.first->type != Tile::SWITCH)
			wireGroup->linkGate(gateAndMode.first);
		else
			gateAndMode.first->linkWire(wireGroup);
	}
}

void Scene::wireToWire(cvr pos, Tile::Type type)
{
	// make sure new wire is in a group
	bool groupFound = false;
	int resultWireGroup = -1; // all groups that need merging will be merged to this one
	for (int i = 0; i < wireGroups.size(); i++)
		for (auto side : Tile::directions)
			if (doesNewWireConnectToWire(pos, wireGroups[i], side)) // if point can be added to group
			{
				if (resultWireGroup != -1 && resultWireGroup != i) // if we need to merge 2 groups
				{
					wireGroups[resultWireGroup].merge(std::move(wireGroups[i]));
					wireGroups.erase(wireGroups.begin() + i);
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
}

void Scene::gateToWire(cvr pos, Tile::Side side)
{
	for (auto& wg : wireGroups)
		if (auto wireType = doesNewGateConnectToWire(pos, wg, side); wireType != Tile::VOID)
		{
			WireGroup* wireGroup = &wg;
			Gate* gate = &gates[pos];
			if(gate->type == Tile::SWITCH)
				gate->linkWire(wireGroup);
			else
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
	for (auto& wg : wireGroups)
		std::cout << "group " << unsigned int(&wg) << " i: " << wg.inputs.size() << " o: " << wg.outputs.size() << std::endl;
	for (auto& g : gates)
		std::cout << "gate " << unsigned int(&g) << " i: " << g.second.inputs.size() << " o: " << g.second.outputs.size() << std::endl;
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
}

void Scene::placeWire(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;

	// make sure wire is assigned to a group
	wireToWire(pos, type);
	// connections to gates may occur
	for (auto side : Tile::directions)
		wireToGate(pos, side, type);

	debug();
}

void Scene::placeGate(cvr pos, Tile::Type type)
{
	if (!tileEmpty(pos))
		return;

	gates.emplace(pos, Gate(type));
	// connections to wires may occur
	for (auto side : Tile::directions)
		gateToWire(pos, side);

	debug();
}

void Scene::placeCross(cvr pos)
{
	if (!tileEmpty(pos))
		return;

	crosses.insert(pos);
}

void Scene::print()
{
	window.clear();
	sf::RectangleShape r(sf::Vector2f(squareSize - 1, squareSize - 1));

	auto stateColor = [this](bool state)
		{
			return state ? sf::Color::White : sf::Color(64, 64, 64);
		};
	auto visible = [this](cvr pos)
	{
			return pos.x >= -squareSize && pos.y >= -squareSize && pos.x < viewport && pos.y < viewport;
		};
	// wires
	for (const auto& wg : wireGroups)
	{
		r.setFillColor(stateColor(wg.outputs.size() > 0)); // wg.state
		for (auto& wireTile : wg.wireTiles)
		{
			if (visible(ctp(wireTile.first)))
			{
			r.setPosition(ctp(wireTile.first));
			r.setTexture(&textures[static_cast<int>(wireTile.second)]);
			window.draw(r);
		}
	}
	}
	// gates
	for (const auto& gate : gates)
	{
		if (visible(ctp(gate.first)))
	{
		r.setPosition(ctp(gate.first));
		r.setFillColor(stateColor(gate.second.outputs.size() > 0)); // gate.second.state
		r.setTexture(&textures[static_cast<int>(gate.second.type)]);
		window.draw(r);
	}
	}
	// crosses
	r.setFillColor(sf::Color::White);
	for (const auto& cross : crosses)
	{
		if (visible(ctp(cross)))
	{
		r.setPosition(ctp(cross));
		r.setTexture(&textures[Tile::CROSS]);
		window.draw(r);
	}
	}
	// lastly, the selection panel
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

void Scene::eventLoop()
{
	sf::Event e;
	clock_t lastFrame = clock();

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
					case Tile::SWITCH:
						placeGate(ptc(sf::Mouse::getPosition(window)), Tile::tiles[selectedTile]);
						break;
					case Tile::WIRE:
					case Tile::INPUT:
						placeWire(ptc(sf::Mouse::getPosition(window)), Tile::tiles[selectedTile]);
						break;
					case Tile::CROSS:
						placeCross(ptc(sf::Mouse::getPosition(window)));
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
