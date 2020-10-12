#pragma once
#include <Game/GameCommunicator.h>
#include <Logging/FileLogger.h>
#include <Configuration/GameConfig.h>

class GameRunner
{
public:
	GameRunner(const SGA::GameConfig& config);

	void runGames(int playerCount, int seed);
	void runGame(const std::vector<int>& agentAssignment, std::mt19937 rngEngine);
	
private:
	const SGA::GameConfig* config;
	int gameCount;
};