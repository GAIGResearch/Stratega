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
		Agent(const Agent& other) = delete;
		Agent(Agent&& other) noexcept = delete;
		Agent& operator=(const Agent& other) = delete;
		Agent& operator=(Agent&& other) noexcept = delete;
		
		virtual void init(GameState initialState, const EntityForwardModel& forwardModel, long timeBudgetMs);
		virtual ActionAssignment computeAction(GameState state, const EntityForwardModel* forwardModel, long timeBudgetMs) = 0;
		virtual void computeAction2(GameState state, const EntityForwardModel* forwardModel, long timeBudgetMs);

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

