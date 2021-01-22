#pragma once
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAction.h>

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
		
		virtual TBSAction getAction(TBSGameState& gameState, std::vector<TBSAction>& actionSpace) const = 0;
		virtual TBSAction getActionForUnit(TBSGameState& gameState, std::vector<TBSAction>& actionSpace, int unitID) const = 0;
		[[nodiscard]] virtual std::string toString() const = 0;
		[[nodiscard]] virtual int getID() const = 0;
		
		friend std::ostream& operator<<(std::ostream& os, const BaseActionScript& dt)
		{
			os << dt.toString();
			return os;
		};
	};
}
