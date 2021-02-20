#include <Stratega/Agent/DoNothingAgent.h>

namespace SGA
{
	void DoNothingAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
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