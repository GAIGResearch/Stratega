#include <Agent/DoNothingAgent.h>

namespace SGA
{
	void DoNothingAgent::runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel)
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