#include "WireGroup.h"
#include "Gate.h"

WireGroup::WireGroup(const WireGroup&& another) : crosses(another.crosses), gates(another.gates)
{
	wireTiles = std::move(another.wireTiles);
	outputs = std::move(another.outputs);
}

WireGroup::WireGroup(const WireGroup& another) : crosses(another.crosses), gates(another.gates)
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

void WireGroup::merge(WireGroup&& another)
{
	wireTiles.merge(another.wireTiles);
	outputs.insert(outputs.end(), another.outputs.begin(), another.outputs.end());
	state |= another.state;
}

WireGroup& WireGroup::operator=(const WireGroup& another)
{
	wireTiles = another.wireTiles;
	outputs = another.outputs;
	state = another.state;
	return *this;
}
