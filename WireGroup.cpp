#include "WireGroup.h"
#include "Gate.h"

WireGroup::WireGroup(std::vector<std::pair<v, Tile::Type>> pos)
	:Tile(Tile::WIRE)
{
	for (auto i = pos.begin(); i != pos.end(); i++)
		wireTiles.insert(*i);
}

WireGroup::WireGroup(v pos, Tile::Type type)
	:Tile(Tile::WIRE)
{
	wireTiles.insert(std::make_pair(pos, type));
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
		output->removeInput(&another);
	}
	for (auto input : another.inputs()) // inherit inputs from another
	{
		addInput(input);
		input->addOutput(this);
		input->removeOutput(&another);
	}
}

