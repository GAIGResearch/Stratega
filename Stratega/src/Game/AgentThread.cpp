#include <cassert>
#include <Stratega/Game/AgentThread.h>

namespace SGA
{

	AgentThread::AgentThread():
		agent(nullptr),
		state(nullptr),
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

	void AgentThread::startComputing(Agent& agent, const GameState& state)
	{
		assert(!computing);
		computing = true;

		// Setup computation
		this->agent = &agent;
		this->state = &state;
		resultCache = AgentResults{};

		// Start thread to compute
		thread = std::thread(&AgentThread::runAgent, this);
	}

	AgentResults AgentThread::join()
	{
		assert(computing);
		computing = false;

		thread.join();
		return resultCache;
	}

	void AgentThread::runAgent()
	{

	}
	
}