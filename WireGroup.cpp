#include "WireGroup.h"
#include "Gate.h"

WireGroup::WireGroup(v pos, Tile::Type type)
	:Tile(Tile::VOID)
{
	wireTiles.insert({ pos, type });
	min = pos;
	max = pos;
}

WireGroup::WireGroup(WireGroup&& another)
	:Tile(Tile::VOID)
{
	_state = another.state(); // copy state
	wireTiles = std::move(another.wireTiles);
	tileOrientations = std::move(another.tileOrientations);
	for (auto input : another.inputs()) // inherit inputs from another
		input->addOutput(this);
	for (auto output : another.outputs()) // inherit outputs from another
		output->addInput(this);
	another.unlinkAll(); // make sure that another is no longer referenced
}

void WireGroup::merge(WireGroup&& another)
{
	_state |= another.state();
	wireTiles.merge(std::move(another.wireTiles)); // merge tiles

	tileOrientations.merge(std::move(another.tileOrientations));

	for (auto output : another.outputs()) // inherit outputs from another
	{
		addOutput(output);
		output->addInput(this);
	}
	for (auto input : another.inputs()) // inherit inputs from another
	{
		addInput(input);
		input->addOutput(this);
	}
	another.unlinkAll(); // make sure that another is no longer referenced
}

