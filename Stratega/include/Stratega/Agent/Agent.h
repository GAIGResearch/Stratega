#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <random>

namespace SGA
{
	class Agent
	{
	public:
		Agent();
		virtual ~Agent() = default;
		
		virtual void init(GameState initialState, EntityForwardModel& forwardModel, long timeBudgetMs);
		virtual ActionAssignment computeAction(GameState state, EntityForwardModel& forwardModel, long timeBudgetMs) = 0;

		int getPlayerID();
		void setPlayerID(int playerID);
		void setSeed(std::mt19937::result_type seed = 0);

	protected:
		std::mt19937& getRNGEngine();

	private:
		int playerID;
		std::mt19937 rngEngine;
	};
}

