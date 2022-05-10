#pragma once
#include <Stratega/Logging/FileLogger.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Game/GameObserver.h>
#include <Stratega/Game/GameRunner.h>

class Arena : public SGA::GameObserver
{
public:
	Arena(const SGA::GameConfig& newConfig, std::unique_ptr<SGA::GameRunner> newRunner)
		: config(&newConfig), runner(std::move(newRunner)), gameBattleCount(0)
	{
	}
	Arena(const SGA::GameConfig& config);

	virtual void runGames(int playerCount, int seed, int gamesNumber, int mapNumber=1);
	virtual void runGames(int playerCount, int seed, int gamesNumber, int mapNumber, std::vector<std::shared_ptr<SGA::Agent>> agents);
	virtual void runGame(const std::vector<int>& agentAssignment, boost::mt19937 rngEngine);
	virtual void runGame(const std::vector<int>& agentAssignment, boost::mt19937 rngEngine, std::vector<std::shared_ptr<SGA::Agent>> agents);

	virtual void onGameStateAdvanced(const SGA::GameState& state, const SGA::ForwardModel& forwardModel) override;
	virtual void onGameFinished(const SGA::GameState& finalState, const SGA::ForwardModel& forwardModel) override;
	
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