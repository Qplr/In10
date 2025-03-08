#include "Gate.h"
#include "WireGroup.h"

Gate::Gate(Gate&& another):
	Tile(another.type())
{
	_state = another.state(); // copy state
	pos = another.pos; // copy pos
	_type = another.type(); // copy type
	for (auto input : another.inputs()) // inherit inputs from another
		input->addOutput(this);
	for (auto output : another.outputs()) // inherit outputs from another
		output->addInput(this);
	another.unlinkAll(); // make sure that another is no longer referenced
}

Gate::Gate(v pos, Type type): Tile(type), pos(pos)
{

}