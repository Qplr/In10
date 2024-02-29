#include "WireGroup.h"
#include "Gate.h"

WireGroup::WireGroup(const WireGroup&& another) : crosses(another.crosses)
{
	*this = another;
}

WireGroup::WireGroup(const WireGroup& another) : crosses(another.crosses)
{
	*this = another;
}

void WireGroup::linkGate(Gate* gate)
{
	outputs.push_back(gate);
	gate->inputs.push_back(this);
}

void WireGroup::unLinkGate(Gate* gate)
{
	for (int i = 0; i < outputs.size(); i++)
		if (gate == outputs[i])
			outputs.erase(outputs.begin() + i);
	for (int i = 0; i < gate->inputs.size(); i++)
		if (gate->inputs[i] == this)
			gate->inputs.erase(gate->inputs.begin() + i);
}

void WireGroup::unlinkAll()
{
	for (auto& input : inputs)
		input->unLinkWire(this);
}

void WireGroup::merge(WireGroup&& another)
{
	wireTiles.merge(another.wireTiles);
	outputs.insert(outputs.end(), another.outputs.begin(), another.outputs.end());
	inputs.insert(inputs.end(), another.inputs.begin(), another.inputs.end());
	state |= another.state;
}

WireGroup& WireGroup::operator=(const WireGroup& another)
{
	wireTiles = another.wireTiles;
	outputs = another.outputs;
	inputs = another.inputs;
	state = another.state;
	return *this;
}

WireGroup& WireGroup::operator=(const WireGroup&& another)
{
	wireTiles = std::move(another.wireTiles);
	outputs = std::move(another.outputs);
	inputs = std::move(another.inputs);
	state = another.state;
	return *this;
}
