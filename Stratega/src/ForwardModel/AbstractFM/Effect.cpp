#include <ForwardModel/AbstractFM/Effect.h>

namespace SGA
{
	AddToResource::AddToResource(FunctionParameter resourceReference, FunctionParameter amount) :
		resourceReference(resourceReference),
		amount(amount)
	{

	}
	
	void AddToResource::execute(GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);
		
		targetResource += amount;
	}
}
