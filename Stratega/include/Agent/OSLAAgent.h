#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;
		
	private:

	};
}