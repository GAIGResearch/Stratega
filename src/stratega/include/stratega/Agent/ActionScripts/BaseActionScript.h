#pragma once
#include <stratega/Representation/GameState.h>
#include <stratega/ForwardModel/Action.h>
#include <stratega/ForwardModel/ActionSpace.h>
#include <map>

namespace SGA {
	class BaseActionScript
	{
	protected:
		std::vector<Action> filterActionTypes(std::vector<Action>& actions, std::string type, std::map<int, std::string> actionTypeIDToActionTypeString) const
		{
			std::vector<Action> filteredActions;

			for (const auto& a : actions)
			{
				if (actionTypeIDToActionTypeString.at(a.getActionTypeID()) == type)
				{
					filteredActions.emplace_back(a);
				}
			}
			return filteredActions;
		}

		std::vector<Action> filterAllButThisActionType(std::vector<Action>& actions, std::string type, std::map<int, std::string> actionTypeIDToActionTypeString) const
		{
			std::vector<Action> filteredActions;

			for (const auto& a : actions)
			{
				if (actionTypeIDToActionTypeString.at(a.getActionTypeID()) != type)
				{
					filteredActions.emplace_back(a);
				}
			}
			return filteredActions;
		}

	public:
		BaseActionScript() = default;
		virtual ~BaseActionScript() = default;

		BaseActionScript(const BaseActionScript&) = default;
		BaseActionScript& operator =(BaseActionScript const&) = default;
		BaseActionScript(BaseActionScript&&) = default;
		BaseActionScript& operator=(BaseActionScript&&) = default;

		virtual Action getAction(const GameState& gameState, std::vector<Action>& actionSpace, int playerID) const = 0;
		virtual Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int playerID, int unitID) const = 0;
		[[nodiscard]] virtual std::string toString() const = 0;

		friend std::ostream& operator<<(std::ostream& os, const BaseActionScript& dt)
		{
			os << dt.toString();
			return os;
		};
	};
}
