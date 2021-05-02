#pragma once
#include <Stratega/Agent/Agent.h>
#include <thread>

namespace SGA
{
	struct AgentResults
	{
		std::exception_ptr error = nullptr;
		ActionAssignment actions{};
		std::chrono::nanoseconds computationTime{};
	};

	AgentResults runAgent(Agent& agent, const GameState& state, const EntityForwardModel& forwardModel);
	
	class AgentThread final
	{
	public:
		AgentThread();
		~AgentThread();
		void startComputing(Agent& agent, const GameState& state, const EntityForwardModel& forwardModel);
		AgentResults join();

	private:
		void runAgentThread();
		
		Agent* agent;
		const GameState* state;
		const EntityForwardModel* forwardModel;
		std::thread thread;
		
		bool computing;
		AgentResults resultCache;
	};
}