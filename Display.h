#pragma once

#include "Scene.h"

class Display
{
	const int viewport = 800, fps = 60, textureSize = 32;
	int squareSize = 25;

	clock_t lastFrame = clock();
	sf::RenderWindow _window;
	std::vector<sf::Texture> textures;

	Scene& _source;
public:
	v ptc(cvr pixels)const;
	v ctp(cvr coords)const;
	Display(Scene& source);
	sf::RenderWindow& window() { return _window; }
	void print();
	void zoom(int delta) { squareSize += delta; }
};

