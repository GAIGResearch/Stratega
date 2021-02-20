#include <Stratega/ForwardModel/Action.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	void Action::execute(GameState& state, const EntityForwardModel& fm) const
	{
		auto& type = state.actionTypes->at(actionTypeID);
		for (auto& effect : type.effects)
		{
			effect->execute(state,fm, targets);
		}
	}
}
