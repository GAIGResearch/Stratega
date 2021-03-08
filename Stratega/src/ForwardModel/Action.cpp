#include <cassert>
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

	bool Action::isEntityAction() const
	{
		return targets.at(0).getType() == ActionTarget::EntityReference;
	}
	
	bool Action::isPlayerAction() const
	{
		return targets.at(0).getType() == ActionTarget::PlayerReference;
	}

	[[nodiscard]] int Action::getSourceID() const
	{
		return isEntityAction() ? targets.at(0).getEntityID() : targets.at(0).getPlayerID();
	}
}
