#include <cassert>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{
	AgentResults runAgent(Agent& agent, const GameState& state, const EntityForwardModel& forwardModel)
	{
		AgentResults results;
		try
		{
			auto begin = std::chrono::high_resolution_clock::now();
			results.actions = agent.computeAction(state, forwardModel, 40);
			auto end = std::chrono::high_resolution_clock::now();
			results.computationTime = end - begin;
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

	void AgentThread::startComputing(Agent& agent, const GameState& state, const EntityForwardModel& forwardModel)
	{
		assert(!computing);
		computing = true;

		// Setup computation
		this->agent = &agent;
		this->state = &state;
		this->forwardModel = &forwardModel;
		resultCache = AgentResults{};

		// Start thread to compute
		thread = std::thread(&AgentThread::runAgentThread, this);
	}

	AgentResults AgentThread::join()
	{
		assert(computing);
		computing = false;

		thread.join();
		return resultCache;
	}

	void AgentThread::runAgentThread()
	{
		resultCache = runAgent(*this->agent, *this->state, *this->forwardModel);
	}

	
}