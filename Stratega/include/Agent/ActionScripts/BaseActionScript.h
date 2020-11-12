#pragma once
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>
#include <ForwardModel/TBSActionSpace.h>

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
		
		virtual Action<Vector2i> getAction(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace) const = 0;
		virtual Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace, int unitID) const = 0;
		[[nodiscard]] virtual std::string toString() const = 0;
		
		friend std::ostream& operator<<(std::ostream& os, const BaseActionScript& dt)
		{
			os << dt.toString();
			return os;
		};
	};
}
