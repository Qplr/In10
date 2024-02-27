#include "Scene.h"
#include "SFML/Graphics.hpp"

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

Scene::Scene()
{
	window.create(sf::VideoMode(800, 800), "");
}

void Scene::placeWire(cvr pos, WireGroup::Type type)
{
	if (!tileEmpty(pos))
		return;

	bool groupFound = false;
	int accumulator = -1; // all groups that need merging will be merged to this one
	for(int i = 0; i < wireGroups.size(); i++)
		if (wireGroups[i].doesPointConnect(pos, type)) // if point can be added to group
		{
			if (accumulator != -1) // if we need to merge 2 groups
			{
				wireGroups[accumulator].merge(std::move(wireGroups[i]));
				wireGroups.erase(wireGroups.begin() + i);
				i--;
			}
			else
			{
				groupFound = true;
				// can be added (but only once makes sense) straight away since we add a wire that belongs there
				wireGroups[i].wireTiles.emplace(pos, type);
				accumulator = i;
			}
		}
	if (!groupFound) // if point was not connected to any group - create new group
	{
		wireGroups.emplace_back(WireGroup(crosses));
		wireGroups.back().wireTiles.emplace(pos, type);
	}
	std::cout << "number of wire groups: " << wireGroups.size() << std::endl;
}

void Scene::placeGate(cvr pos, Gate::Type type)
{
	if (!tileEmpty(pos))
		return;
	
	gates.emplace(pos, Gate(type));
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
	sf::RectangleShape r(sf::Vector2f(squareSize, squareSize));

	for(int i = 0; i < viewport/squareSize; i++)
		for (int j = 0; j < viewport / squareSize; j++)
		{
			for (int k = 0; k < wireGroups.size(); k++)
				if (wireGroups[k].wireTiles.contains(v(i, j)))
				{
					switch (k % 5)
					{
					case 0:
						r.setFillColor(sf::Color::White);
						break;
					case 1:
						r.setFillColor(sf::Color::Red);
						break;
					case 2:
						r.setFillColor(sf::Color::Green);
						break;
					case 3:
						r.setFillColor(sf::Color::Blue);
						break;
					case 4:
						r.setFillColor(sf::Color::Yellow);
						break;
					}
					r.setPosition(i * squareSize, j * squareSize);
					window.draw(r);
				}
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
			if (e.type == sf::Event::MouseButtonPressed)
			{
				if(e.key.code == sf::Mouse::Left)
					placeWire(sf::Mouse::getPosition(window) / squareSize, WireGroup::WIRE);
			}
		}
		if(clock() > lastFrame + CLOCKS_PER_SEC / fps)
		{
			lastFrame = clock();
			print();
		}
	}
}
