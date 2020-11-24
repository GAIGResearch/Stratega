#include <ForwardModel/AbstractFM/Action.h>
#include <Representation/AbstractGS/GameState.h>

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
}
