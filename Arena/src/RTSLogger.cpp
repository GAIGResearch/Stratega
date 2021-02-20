#include <RTSLogger.h>
#include <Stratega/Logging/Log.h>

void RTSLogger::close()
{
	SGA::Log::logSingleValue("WinnerID", game->getState().winnerPlayerID);
}

void RTSLogger::onGameStateAdvanced()
{
	const auto& state = game->getState();
	for(size_t i = 0; i < state.players.size(); i++)
	{
		int playerID = state.players[i].id;
		SGA::LoggingScope playerScope("Player" + std::to_string(playerID));
		// ToDo getActions should accept const gameStates
		auto actions = game->getForwardModel().generateActions(const_cast<SGA::RTSGameState&>(state), playerID);
		SGA::Log::logValue("ActionCount", actions.size());
	}
}