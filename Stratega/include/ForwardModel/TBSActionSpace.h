#pragma once
#include <ForwardModel/ActionSpace.h>

namespace SGA
{
	class TBSUnit;
	template <class T=Vector2f>
	class ActionSpace : public ActionSpaceBase<Action<T>>
	{
	public:
		typename std::vector<Action<T>>::const_iterator begin() const
		{
			return actions.begin();
		}
		typename std::vector<Action<T>>::const_iterator end() const
		{
			return actions.end();
		}

		void emplace_back(Action<T> action)
		{
			actions.emplace_back(action);
		}
		Action<T> at(int index)const
		{
			return	actions[index];
		}

		[[nodiscard]] size_t size() const { return actions.size(); }
		void clear() { actions.clear(); }

		ActionSpace filterUnitActions(TBSUnit& unit)
		{
			ActionSpace subSpace;
			for (const auto& a : actions)
			{
				if (a.getSourceUnitID() == unit.getUnitID())
				{
					subSpace.emplace_back(a);
				}
			}

			return subSpace;
		}
		ActionSpace filterActionTypes(ActionType type)
		{
			ActionSpace subSpace;
			for (const auto& a : actions)
			{
				if (a.getType() == type)
				{
					subSpace.emplace_back(a);
				}
			}

			return subSpace;
		}

	private:
		std::vector<Action<T>> actions;

	};
}