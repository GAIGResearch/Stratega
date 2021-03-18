#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	void DoNothingAgent::runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				gameCommunicator.executeAction(Action::createEndAction(gameCommunicator.getPlayerID()));
			}
		}
	}
}