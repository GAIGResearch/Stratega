#pragma once
#include <Stratega/Representation/TBSGameState.h>
#include <Stratega/ForwardModel/Action.h>

namespace SGA {
	class BaseActionScript
	{

	public:
		BaseActionScript() = default;
		virtual ~BaseActionScript() = default;
		
		BaseActionScript(const BaseActionScript&) = default;
		BaseActionScript& operator =(BaseActionScript const&) = default;
		BaseActionScript(BaseActionScript&&) = default;
		BaseActionScript& operator=(BaseActionScript&&) = default;
		
		virtual Action getAction(TBSGameState& gameState, std::vector<Action>& actionSpace) const = 0;
		virtual Action getActionForUnit(TBSGameState& gameState, std::vector<Action>& actionSpace, int unitID) const = 0;
		[[nodiscard]] virtual std::string toString() const = 0;
		
		friend std::ostream& operator<<(std::ostream& os, const BaseActionScript& dt)
		{
			os << dt.toString();
			return os;
		};
	};
}
