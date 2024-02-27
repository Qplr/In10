#pragma once

#include "SFML/Graphics.hpp"

#include <map>

#include "defines.h"

class WireGroup;

class Gate
{
public:
	enum Type
	{
		OR, NOR, AND, NAND, XOR, XNOR
	};
private:
	int id;
	bool state;
	Type type;
	std::map<v, WireGroup*> outputs;
public:
	Gate(Type type);
	friend class Scene;
};

