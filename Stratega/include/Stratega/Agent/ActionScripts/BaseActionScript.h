#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/Action.h>
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
				if (actionTypeIDToActionTypeString.at(a.actionTypeID) == type)
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
				if (actionTypeIDToActionTypeString.at(a.actionTypeID) != type)
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
		
		virtual Action getAction(const GameState& gameState, std::vector<Action>& actionSpace) const = 0;
		virtual Action getActionForUnit(const GameState& gameState, std::vector<Action>& actionSpace, int unitID) const = 0;
		[[nodiscard]] virtual std::string toString() const = 0;
		
		friend std::ostream& operator<<(std::ostream& os, const BaseActionScript& dt)
		{
			os << dt.toString();
			return os;
		};
	};
}
