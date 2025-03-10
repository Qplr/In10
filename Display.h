#pragma once

#include "Scene.h"

class Display
{
	const int fps = 60, textureSize = 32;
	const float maxSquareSize = 128, minSquareSize = 8;
	float squareSize = textureSize, targetSquareSize = squareSize;
	sf::Vector2f camPosUnits = {1.f, 1.f};
	void cameraBounds();

	clock_t lastFrame = clock();
	sf::RenderWindow _window;
	std::vector<sf::Texture> textures;

	Scene& _source;
public:
	void resize() { _window.create(sf::VideoMode(_window.getSize().x, _window.getSize().y), ""); }
	sf::Vector2f ptc(cvr pixels)const;
	v ctp(const sf::Vector2f& coords)const;
	Display(Scene& source);
	sf::RenderWindow& window() { return _window; }
	void print();
	void zoom(int delta);
	void move(v offsetPixels);
};

