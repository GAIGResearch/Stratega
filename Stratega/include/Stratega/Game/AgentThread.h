#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	struct AgentResults
	{
		std::exception_ptr agentError;
		ActionAssignment actions;
		double computationTime;
	};
	
	class AgentThread final
	{
	public:
		AgentThread();
		~AgentThread();
		void startComputing(Agent& agent, const GameState& state);
		AgentResults join();

	private:
		void runAgent();
		
		Agent* agent;
		const GameState* state;
		std::thread thread;
		
		bool computing;
		AgentResults resultCache;
	};
}