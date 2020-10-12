#include <ForwardModel/TBSActionSpace.h>
#include <Representation/TBSUnit.h>

namespace SGA
{
	/*std::vector<Action>::const_iterator ActionSpace::begin() const
	{
		return actions.begin();
	}

	std::vector<Action>::const_iterator ActionSpace::end() const
	{
		return actions.end();
	}*/

	//void ActionSpace::addAction(Action action)
	//{
	//	actions.emplace_back(action);
	//}

	/*ActionSpace ActionSpace::filterUnitActions(TBSUnit& unit)
	{
		ActionSpace subSpace;
		for(const auto& a : actions)
		{
			if(a.getSourceUnitID() == unit.getUnitID())
			{
				subSpace.addAction(a);
			}
		}

		return subSpace;
	}*/
	
	/*ActionSpace ActionSpace::filterActionTypes(ActionType type)
	{
		ActionSpace subSpace;
		for (const auto& a : actions)
		{
			if (a.getType() == type)
			{
				subSpace.addAction(a);
			}
		}

		return subSpace;
	}*/
}
