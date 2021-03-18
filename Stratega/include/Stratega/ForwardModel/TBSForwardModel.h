#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>

namespace  SGA
{
	struct GameState;
	
	class TBSForwardModel : public EntityForwardModel
	{
	public:
		void advanceGameState(GameState& state, const Action& action) const;

		void endTurn(GameState& state) const;

		virtual std::vector<Action> generateActions(GameState& state) const;

		virtual std::vector<Action> generateActions(GameState& state, int playerID) const;

		virtual bool isValid(const GameState& state, const Action& action) const;

		bool checkGameIsFinished(GameState& state) const;
	};
}
