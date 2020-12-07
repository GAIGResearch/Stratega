#include <ForwardModel/AbstractFM/Action.h>
#include <Representation/AbstractGS/GameState.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>


namespace SGA
{
	void Action::execute(GameState& state) const
	{
		auto& type = state.actionTypes->at(actionTypeID);
		for(auto& effect : type.effects)
		{
			effect->execute(state, targets);
		}
	}

	void Action::execute(GameState& state, const TBSAbstractForwardModel& fm) const
	{
		auto& type = state.actionTypes->at(actionTypeID);
		for (auto& effect : type.effects)
		{
			effect->executeTBS(state,fm, targets);
		}
	}

}
