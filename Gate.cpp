#include "Gate.h"

Gate::Gate(Type type): type(type)
{

}

void Gate::linkWire(WireGroup* wg)
{
	outputs.push_back(wg);
}

void Gate::unLinkWire(WireGroup* wg)
{
	for (int i = 0; i < outputs.size(); i++)
		if (wg == outputs[i])
			outputs.erase(outputs.begin() + i);
}
