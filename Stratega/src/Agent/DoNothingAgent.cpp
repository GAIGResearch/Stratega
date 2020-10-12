#include <Agent/DoNothingAgent.h>

namespace SGA
{
	void DoNothingAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
				gameCommunicator.executeAction(Action<Vector2i>(SGA::ActionType::EndTurn, gameCommunicator.getPlayerID()));

		}
	}
}