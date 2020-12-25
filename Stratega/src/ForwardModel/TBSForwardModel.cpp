#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{
	bool TBSForwardModel::canPlayerPlay(Player& player) const
	{
		if (player.state.get().fogOfWarId != -1 && player.id != player.state.get().fogOfWarId)
			return true;

		switch (winCondition)
		{
		case WinConditionType::UnitAlive:
		{
			//player.getEntities();
			bool hasKing = false;
			std::vector<Entity*> units = player.getEntities();

			for (auto& unit : units)
			{
				//Check if player has units
				if (unit->typeID == unitTypeID)
				{
					hasKing = true;
				}
			}

			if (!hasKing)
			{
				return false;
			}

			break;
		}
		case WinConditionType::LastManStanding:
		{
			std::vector<Entity*> units = player.getEntities();

			if (units.empty())
			{
				return false;
			}
			break;
		}
		}

		return true;
	}

}
