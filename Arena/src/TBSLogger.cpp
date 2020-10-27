#pragma once
#include <TBSLogger.h>
#include <Logging/Log.h>

void TBSLogger::close()
{
	SGA::Log::logSingleValue("WinnerID", this->game->getState().getWinnerID());
	SGA::Log::logSingleValue("Turns", this->game->getState().currentGameTurn + 1);
}

void TBSLogger::onGameStateAdvanced()
{
	SGA::Log::logValue("ActivePlayer", this->game->getState().currentPlayer);

	// ToDo getActions should accept const gameStates
	auto actions = this->game->getForwardModel().getActions(const_cast<SGA::TBSGameState&>(this->game->getState()));
	SGA::Log::logValue("ActionCount", actions->count());
}