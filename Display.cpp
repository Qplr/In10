#include <format>

#include "Display.h"

void Display::cameraBounds()
{
	camPosUnits.x = std::max(camPosUnits.x, 1.f);
	camPosUnits.y = std::max(camPosUnits.y, 1.f);
}

sf::Vector2f Display::ptc(cvr pixels) const
{
	return sf::Vector2f(pixels) / squareSize + camPosUnits;
}

v Display::ctp(const sf::Vector2f& coords) const
{
	return (coords - camPosUnits) * squareSize;
}

Display::Display(Scene& source): _source(source)
{
	_window.create(sf::VideoMode(800, 800), "");
	_window.setVerticalSyncEnabled(true);

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
#include <iostream>
void Display::print()
{
	if (abs(squareSize - targetSquareSize) > 0.0001)
	{
		auto pivotUnits = sf::Vector2f(sf::Mouse::getPosition(_window)) / squareSize;
		squareSize += (targetSquareSize > squareSize ? 1 : -1);
		camPosUnits += pivotUnits - sf::Vector2f(sf::Vector2f(sf::Mouse::getPosition(_window))) / squareSize;
		std::cout << "square size: " << squareSize << " target: " << targetSquareSize << std::endl;
	}

	_window.clear();
	sf::RectangleShape r(sf::Vector2f(squareSize - 1, squareSize - 1));
#ifdef _DEBUG
	sf::RectangleShape ro(sf::Vector2f(4, 4));
	ro.setOrigin(-squareSize / 2 + 2, -squareSize / 2 + 2);
#endif

	auto stateColor = [](int state)
		{
			return state ? sf::Color::White : sf::Color(64, 64, 64);
		};
	auto visible = [this](cvr pos)
		{
			return pos.x >= -squareSize && pos.y >= -squareSize && pos.x < float(_window.getSize().x) && pos.y < float(_window.getSize().y);
		};
	// wires
	for (auto wg : _source.wireGroups())
	{
		r.setFillColor(stateColor(wg->state()));
		for (auto& wireTile : wg->wireTiles())
		{
			if (auto wirePos = ctp(sf::Vector2f(wireTile.first)); visible(wirePos))
			{
				r.setPosition(wirePos);
				r.setTexture(&textures[static_cast<int>(wireTile.second)]);
				_window.draw(r);
			}
		}
#ifdef _DEBUG
		for (auto& orientation : wg->tileOrientations())
		{
			auto pixelcoords = ctp(sf::Vector2f(orientation.first));
			if (orientation.second.hasConnection(Tile::N))
			{
				ro.setPosition(pixelcoords.x, pixelcoords.y - 8);
				_window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::S))
			{
				ro.setPosition(pixelcoords.x, pixelcoords.y + 8);
				_window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::W))
			{
				ro.setPosition(pixelcoords.x - 8, pixelcoords.y);
				_window.draw(ro);
			}
			if (orientation.second.hasConnection(Tile::E))
			{
				ro.setPosition(pixelcoords.x + 8, pixelcoords.y);
				_window.draw(ro);
			}
		}
#endif
	}
	// gates
	for (auto gate : _source.gates())
	{
		if (auto gatePos = ctp(sf::Vector2f(gate->pos())); visible(gatePos))
		{
			r.setPosition(gatePos);
			r.setFillColor(stateColor(gate->state()));
			r.setTexture(&textures[static_cast<int>(gate->type())]);
			_window.draw(r);
		}
#ifdef _DEBUG
		auto pixelcoords = ctp(sf::Vector2f(gate->pos()));
		if (gate->orientation().hasConnection(Tile::N))
		{
			ro.setPosition(pixelcoords.x, pixelcoords.y - 8);
			_window.draw(ro);
		}
		if (gate->orientation().hasConnection(Tile::S))
		{
			ro.setPosition(pixelcoords.x, pixelcoords.y + 8);
			_window.draw(ro);
		}
		if (gate->orientation().hasConnection(Tile::W))
		{
			ro.setPosition(pixelcoords.x - 8, pixelcoords.y);
			_window.draw(ro);
		}
		if (gate->orientation().hasConnection(Tile::E))
		{
			ro.setPosition(pixelcoords.x + 8, pixelcoords.y);
			_window.draw(ro);
		}
#endif
	}
	// crosses
	r.setFillColor(sf::Color::White);
	for (auto cross : _source.crosses())
	{
		if (auto crossPos = ctp(sf::Vector2f(cross)); visible(crossPos))
		{
			r.setPosition(crossPos);
			r.setTexture(&textures[Tile::CROSS]);
			_window.draw(r);
		}
	}
	// the selection panel
	r.setSize(v(32, 32));
	r.setOutlineThickness(1);
	for (int i = 0; i < textures.size(); i++)
	{
		r.setTexture(&textures[i]);
		r.setPosition(4, 4 + i * 36);
		r.setFillColor(stateColor(_source.selectedTile() == Tile::tiles[i]));
		_window.draw(r);
	}

	_window.display();
}

void Display::zoom(int delta)
{
	targetSquareSize += int(delta * log(targetSquareSize));
	targetSquareSize = std::min(targetSquareSize, maxSquareSize);
	targetSquareSize = std::max(targetSquareSize, minSquareSize);
}

void Display::move(v offsetPixels)
{
	camPosUnits += sf::Vector2f(offsetPixels) / squareSize;
	cameraBounds();
}
