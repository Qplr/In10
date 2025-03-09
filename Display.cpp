#include <format>

#include "Display.h"

v Display::ptc(cvr pixels) const
{
	return pixels / squareSize;
}

v Display::ctp(cvr coords) const
{
	return sf::Vector2i(coords) * squareSize;
}

Display::Display(Scene& source): _source(source)
{
	_window.create(sf::VideoMode(800, 800), "");
	_window.setFramerateLimit(fps);
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


void Display::print()
{
	if (clock() > lastFrame + CLOCKS_PER_SEC / fps)
		lastFrame = clock();
	else
		return;

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
			return pos.x >= -squareSize && pos.y >= -squareSize && pos.x < viewport && pos.y < viewport;
		};
	// wires
	for (auto wg : _source.wireGroups())
	{
		r.setFillColor(stateColor(wg->state()));
		for (auto& wireTile : wg->wireTiles())
		{
			if (visible(ctp(wireTile.first)))
			{
				r.setPosition(ctp(wireTile.first));
				r.setTexture(&textures[static_cast<int>(wireTile.second)]);
				_window.draw(r);
			}
		}
#ifdef _DEBUG
		for (auto& orientation : wg->tileOrientations())
		{
			auto pixelcoords = ctp(orientation.first);
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
		if (visible(ctp(gate->pos())))
		{
			r.setPosition(ctp(gate->pos()));
			r.setFillColor(stateColor(gate->state()));
			r.setTexture(&textures[static_cast<int>(gate->type())]);
			_window.draw(r);
		}
#ifdef _DEBUG
		auto pixelcoords = ctp(gate->pos());
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
		if (visible(ctp(cross)))
		{
			r.setPosition(ctp(cross));
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