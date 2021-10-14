#pragma once
#include <stratega/Logging/FileLogger.h>
#include <stratega/Configuration/GameConfig.h>
#include <stratega/Game/GameObserver.h>
#include <stratega/Game/GameRunner.h>

class Arena : public SGA::GameObserver
{
public:
	Arena(const SGA::GameConfig& config);

	void runGames(int playerCount, int seed, int gamesNumber, int mapNumber=1);
	void runGames(int playerCount, int seed, int gamesNumber, int mapNumber, std::vector<std::shared_ptr<SGA::Agent>> agents);
	void runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine);
	void runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine, std::vector<std::shared_ptr<SGA::Agent>> agents);

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