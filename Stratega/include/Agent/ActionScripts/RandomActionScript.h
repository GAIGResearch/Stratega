#pragma once
#include <Agent/ActionScripts/BaseActionScript.h>
#include <ForwardModel/Action.h>
#include <ForwardModel/ActionSpace.h>


namespace SGA {
	class RandomActionScript : public BaseActionScript
	{

	public:
		RandomActionScript() : BaseActionScript() {};

		Action<Vector2i> getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace) const override;
		Action<Vector2i> getActionForUnit(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace, int unitID) const override;
		[[nodiscard]] std::string toString() const override { return "RandomActionScript"; };
	};
}
