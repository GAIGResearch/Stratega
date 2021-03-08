#pragma once
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/Representation/TBSGameState.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/ForwardModel/EntityActionSpace.h>

namespace  SGA
{
	class TBSForwardModel : public EntityForwardModel
	{
	public:
		void advanceGameState(TBSGameState& state, const Action& action) const;

		void endTurn(TBSGameState& state) const;

		virtual std::vector<Action> generateActions(TBSGameState& state) const;

		virtual std::vector<Action> generateActions(TBSGameState& state, int playerID) const;

		virtual bool isValid(const TBSGameState& state, const Action& action) const;

		bool checkGameIsFinished(TBSGameState& state) const;
	};
}
