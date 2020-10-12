#include <Representation/Player.h>

namespace SGA
{
	TBSPlayer::TBSPlayer(TBSGameState& state, int playerID)
		: state(state), playerID(playerID), score(0)
	{
	}

	
	std::vector<TBSUnit*> TBSPlayer::getUnits() const
	{
		std::vector<TBSUnit*> unitsOfPlayer;
		for (TBSUnit& unit : state.get().getUnits())
		{
			if (unit.getPlayerID() == this->playerID)
			{
				unitsOfPlayer.emplace_back(&unit);
			}
		}
		return unitsOfPlayer;
	}

	RTSPlayer::RTSPlayer(RTSGameState& state, int playerID)
		: state(state), playerID(playerID), score(0)
	{
	}


	std::vector<RTSUnit*> RTSPlayer::getUnits() const
	{
		std::vector<RTSUnit*> unitsOfPlayer;
		for (RTSUnit& unit : state.get().units)
		{
			if (unit.playerID == this->playerID)
			{
				unitsOfPlayer.emplace_back(&unit);
			}
		}
		return unitsOfPlayer;
	}
}
