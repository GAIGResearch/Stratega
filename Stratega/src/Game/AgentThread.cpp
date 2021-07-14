#include <cassert>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/Utils/Timer.h>

namespace SGA
{
	AgentResults runAgent(Agent& agent, const GameState& state, const ForwardModel& forwardModel, const GameConfig& gameConfig, long timeBudgetMs)
	{
		assert(0 <= agent.getPlayerID() && agent.getPlayerID() < state.players.size());

		auto stateCopy(state);

		//Apply fog of war
		if(gameConfig.applyFogOfWar)
			stateCopy.applyFogOfWar(agent.getPlayerID());

		AgentResults results;
		try
		{
			auto begin = std::chrono::high_resolution_clock::now();
			results.actions = agent.computeAction(std::move(stateCopy), forwardModel, Timer(timeBudgetMs));
			auto end = std::chrono::high_resolution_clock::now();
			//results.computationTime = end - begin;
			results.computationTime = std::chrono::duration_cast<std::chrono::milliseconds>
				(end - begin);
			
		}
		catch (...)
		{
			results.error = std::current_exception();
		}
		return results;
	}
	
	AgentThread::AgentThread():
		agent(nullptr),
		state(nullptr),
		forwardModel(nullptr),
		gameConfig(nullptr),
		computing(false),
		resultCache()
	{
	}

	AgentThread::~AgentThread()
	{
		if (computing)
		{
			thread.join();
		}
	}

	void AgentThread::startComputing(Agent& agent, const GameState& state, const ForwardModel& forwardModel, const GameConfig& gameConfig, long timeBudgetMs)
	{
		// ToDo actually reuse the thread instead of starting a new one everytime
		assert(!computing);
		computing = true;

		// Setup computation
		this->agent = &agent;
		this->state = &state;
		this->forwardModel = &forwardModel;
		this->gameConfig = &gameConfig;
		resultCache = AgentResults{};

		// Start thread to compute
		thread = std::thread(&AgentThread::runAgentThread, this, timeBudgetMs);
	}

	AgentResults AgentThread::join()
	{
		assert(computing);
		computing = false;

		thread.join();
		return resultCache;
	}

	void AgentThread::runAgentThread(long timeBudgetMs)
	{
		resultCache = runAgent(*this->agent, *this->state, *this->forwardModel, *this->gameConfig, timeBudgetMs);
	}

	
}