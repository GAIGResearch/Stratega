#include <cassert>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/Utils/Timer.h>
#pragma warning(disable: 5039)
namespace SGA
{
	AgentResults runAgent(Agent& agent, const GameState& state, const ForwardModel& forwardModel, const GameConfig& gameConfig, long timeBudgetMs)
	{
		assert(0 <= agent.getPlayerID() && agent.getPlayerID() < state.getNumPlayers());

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
		joined(true),
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

	void AgentThread::startComputing(Agent& newAgent, const GameState& newState, const ForwardModel& newForwardModel, const GameConfig& newGameConfig, long timeBudgetMs)
	{
		// ToDo actually reuse the thread instead of starting a new one everytime
		assert(!computing);
		assert(joined);
		computing = true;
		joined = false;

		// Setup computation
		this->agent = &newAgent;
		this->state = &newState;
		this->forwardModel = &newForwardModel;
		this->gameConfig = &newGameConfig;
		resultCache = AgentResults{};
		

		// Start thread to compute
		thread = std::thread(&AgentThread::runAgentThread, this, timeBudgetMs);
	}

	AgentResults AgentThread::join()
	{
		assert(!joined);
		thread.join();
		joined = true;
		return resultCache;
	}

	void AgentThread::runAgentThread(long timeBudgetMs)
	{
		resultCache = runAgent(*this->agent, *this->state, *this->forwardModel, *this->gameConfig, timeBudgetMs);
		computing = false;
	}

	
}