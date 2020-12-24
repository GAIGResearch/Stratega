#include <ForwardModel/Action.h>
#include <Representation/GameState.h>
#include <ForwardModel/TBSAbstractForwardModel.h>


namespace SGA
{
	

	void Action::execute(GameState& state, const TBSAbstractForwardModel& fm) const
	{
		auto& type = state.actionTypes->at(actionTypeID);
		for (auto& effect : type.effects)
		{
			effect->executeTBS(state,fm, targets);
		}
	}

	void Action::execute(GameState& state, const RTSAbstractForwardModel& fm) const
	{
		if (actionTypeID == -1)
			return;
		auto& type = state.actionTypes->at(actionTypeID);
		for (auto& effect : type.effects)
		{
			effect->executeRTS(state, fm, targets);
		}
	}


}
