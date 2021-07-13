#include <cassert>
#include <Stratega/Game/AgentThread.h>
#include <Stratega/Utils/Timer.h>

namespace SGA
{
	AgentResults runAgent(Agent& agent, const GameState& state, const ForwardModel& forwardModel, long timeBudgetMs)
	{
		assert(0 <= agent.getPlayerID() && agent.getPlayerID() < state.getNumPlayers());

		auto stateCopy(state);
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

	void AgentThread::startComputing(Agent& agent, const GameState& state, const ForwardModel& forwardModel, long timeBudgetMs)
	{
		// ToDo actually reuse the thread instead of starting a new one everytime
		assert(!computing);
		computing = true;

		// Setup computation
		this->agent = &agent;
		this->state = &state;
		this->forwardModel = &forwardModel;
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
		resultCache = runAgent(*this->agent, *this->state, *this->forwardModel, timeBudgetMs);
	}

	
}