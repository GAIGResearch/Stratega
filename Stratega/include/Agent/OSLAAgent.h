#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		
	private:

	};
}