#include "Gate.h"
#include "WireGroup.h"

Gate::Gate(Type type): type(type)
{

}

void Gate::linkWire(WireGroup* wg)
{
	outputs.push_back(wg);
	wg->inputs.push_back(this);
}

void Gate::unLinkWire(WireGroup* wg)
{
	for (int i = 0; i < outputs.size(); i++)
		if (wg == outputs[i])
			outputs.erase(outputs.begin() + i);
	for (int i = 0; i < wg->inputs.size(); i++)
		if (wg->inputs[i] == this)
			wg->inputs.erase(wg->inputs.begin() + i);
}

void Gate::unlinkAll()
{
	for (auto& input : inputs)
		input->unLinkGate(this);
}
