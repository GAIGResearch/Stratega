#pragma once
#include <Agent/Agent.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>


namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		
	private:

	};
}