#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		void runAbstractTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		
	private:

	};
}