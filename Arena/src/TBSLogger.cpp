#include <TBSLogger.h>
#include <Stratega/Logging/Log.h>

void TBSLogger::close()
{
	SGA::Log::logSingleValue("WinnerID", this->game->getState().winnerPlayerID);
	SGA::Log::logSingleValue("Turns", this->game->getState().currentTick + 1);
}

void TBSLogger::onGameStateAdvanced()
{
	SGA::Log::logValue("ActivePlayer", this->game->getState().currentPlayer);

	// ToDo getActions should accept const gameStates
	auto actions = this->game->getForwardModel().generateActions(const_cast<SGA::TBSGameState&>(this->game->getState()));
	SGA::Log::logValue("ActionCount", actions.size());
}