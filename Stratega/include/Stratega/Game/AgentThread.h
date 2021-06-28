#pragma once
#include <Stratega/Agent/Agent.h>
#include <thread>

namespace SGA
{
	struct AgentResults
	{
		std::exception_ptr error = nullptr;
		ActionAssignment actions{};
		std::chrono::milliseconds computationTime{};
	};

	/// <summary>
	/// Executes the given agent, measures how long the agent computes and captures any errors.
	/// Note that the game state is not passed directly to the agent. Instead, Fog of War is applied, based on the agents playerID.
	/// </summary>
	/// <param name="agent">The agent that should be used for computing the action. Note that the agent has to be initialized before passing it to this method.</param>
	/// <param name="state">The game state that the agent should use to compute the action.</param>
	/// <param name="forwardModel">The forward model that the agent should use to compute the action.</param>
	/// <returns>The results obtained by executing the agent.</returns>
	AgentResults runAgent(Agent& agent, const GameState& state, const ForwardModel& forwardModel, long timeBudgetMs);

	/// <summary>
	/// A reusable thread for running a given agent.
	/// </summary>
	class AgentThread final
	{
	public:
		AgentThread();
		~AgentThread();
		/// <summary>
		/// Executes the agent in an separate thread, to obtain the results use <see cref="AgentThread::join()">.
		/// </summary>
		/// <param name="agent">The agent that should be used for computing the action. Note that the agent has to be initialized before passing it to this method.</param>
		/// <param name="state">The game state that the agent should use to compute the action.</param>
		/// <param name="forwardModel">The forward model that the agent should use to compute the action.</param>
		void startComputing(Agent& agent, const GameState& state, const ForwardModel& forwardModel, long timeBudgetMs);
		/// <summary>
		/// Waits for the agent-thread to finish computing.
		/// </summary>
		/// <returns>The results obtained by executing the agent.</returns>
		AgentResults join();

	private:
		void runAgentThread(long timeBudgetMs);
		
		Agent* agent;
		const GameState* state;
		const ForwardModel* forwardModel;
		std::thread thread;
		
		bool computing;
		AgentResults resultCache;
	};
}