#pragma once
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	class GameObserver
	{
	public:
		virtual ~GameObserver() = default;
		
		virtual void onGameStarted(const GameState& initialState, const EntityForwardModel& forwardModel) {};
		virtual void onGameStateAdvanced(const GameState& state, const EntityForwardModel& forwardModel) {};
		virtual void onGameFinished(const GameState& finalState, const EntityForwardModel& forwardModel) {};
	};
}