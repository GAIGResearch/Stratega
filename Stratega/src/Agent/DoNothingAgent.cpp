#include <Agent/DoNothingAgent.h>

namespace SGA
{
	void DoNothingAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
				gameCommunicator.executeAction(TBSAction(TBSActionType::EndTurn, gameCommunicator.getPlayerID()));

		}
	}
}