#include "WireGroup.h"
#include "Gate.h"

WireGroup::WireGroup(std::vector<std::pair<v, Tile::Type>> pos)
	:Tile(Tile::WIRE)
{
	for (auto i = pos.begin(); i != pos.end(); i++)
		_wireTiles.insert(*i);
}

WireGroup::WireGroup(v pos, Tile::Type type)
	:Tile(Tile::WIRE)
{
	_wireTiles.insert(std::make_pair(pos, type));
}

void WireGroup::merge(WireGroup&& another)
{
	_state |= another.state();
	_wireTiles.merge(std::move(another._wireTiles)); // merge tiles
	_tileOrientations.merge(std::move(another._tileOrientations));

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

