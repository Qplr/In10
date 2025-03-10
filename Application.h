#pragma once

#include "Scene.h"
#include "Display.h"

class Application
{
	Scene scene;
	Display display;
public:
	Application();
	void eventLoop();
};

