#pragma once
#include <Stratega/Logging/FileLogger.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Game/GameObserver.h>
#include <Stratega/Game/GameRunner.h>

class Arena : public SGA::GameObserver
{
public:
	Arena(const SGA::GameConfig& config);

	void runGames(int playerCount, int seed, int gamesNumber, int mapNumber=1);
	void runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine);

	void onGameStateAdvanced(const SGA::GameState& state, const SGA::ForwardModel& forwardModel) override;
	void onGameFinished(const SGA::GameState& finalState, const SGA::ForwardModel& forwardModel) override;
	
private:
	const SGA::GameConfig* config;
	std::unique_ptr<SGA::GameRunner> runner;
	//Number of battle in the same game combination with exchanged players
	int gameBattleCount;
	//One game is a set of battles between two or more agents
	int gameCount;
	int currentMapID;
	int currentSeed;
};