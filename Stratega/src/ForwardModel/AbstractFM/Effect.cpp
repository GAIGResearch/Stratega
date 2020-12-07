#include <ForwardModel/AbstractFM/Effect.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>

namespace SGA
{
	AddToResource::AddToResource(FunctionParameter resourceReference, FunctionParameter amount) :
		resourceReference(resourceReference),
		amount(amount)
	{

	}
	void AddToResource::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS Add to resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;
	}
	void AddToResource::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute RTS Add to resource" << std::endl;
		auto& targetResource = resourceReference.getParameter(state, targets);
		double amount = this->amount.getConstant(state, targets);

		targetResource += amount;
	}


	void EndTurn::executeTBS(GameState& state, const TBSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		std::cout << "Execute TBS end turn" << std::endl;
		auto& casted=dynamic_cast<TBSGameState2&>(state);
		fm.endTurn(casted);
	}

	void EndTurn::executeRTS(GameState& state, const RTSAbstractForwardModel& fm, const std::vector<ActionTarget>& targets) const
	{
		//NOT NEEDED
	}


}
