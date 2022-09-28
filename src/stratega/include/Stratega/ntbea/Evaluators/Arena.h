#pragma once
#include <Stratega/Logging/FileLogger.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Game/GameObserver.h>
#include <Stratega/Game/GameRunner.h>
//#include <Evaluators/AbstractGameStateMCTSEvaluator.h>

namespace SGA 
{
	class Arena : public SGA::GameObserver
	{
	public:
		Arena(const SGA::GameConfig& config);

		std::vector<int> runGames(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, int playerCount, int seed, int gamesNumber, int mapNumber=1);
		void runGame(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, const std::vector<int>& agentAssignment, boost::mt19937 rngEngine);

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

		std::vector<int> game_results;
	};
}