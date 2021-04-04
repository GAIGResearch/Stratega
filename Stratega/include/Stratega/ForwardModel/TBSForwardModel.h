#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>

namespace  SGA
{
	struct GameState;
	
	class TBSForwardModel : public EntityForwardModel
	{
	public:
		void advanceGameState(GameState& state, const Action& action) const override;
		void advanceGameState(GameState& state, const std::vector<Action>& action) const override;

		using EntityForwardModel::generateActions;
		[[nodiscard]] std::vector<Action> generateActions(const GameState& state) const;
		
		void endTurn(GameState& state) const;
		virtual bool isValid(const GameState& state, const Action& action) const;
		bool checkGameIsFinished(GameState& state) const;
	};
}
