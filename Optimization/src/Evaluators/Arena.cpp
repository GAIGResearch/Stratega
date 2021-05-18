#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Logging/Log.h>
#include <Stratega/Agent/Agent.h>

#include <Evaluators/Arena.h>
#include <utils.h>

namespace SGA
{

	Arena::Arena(const SGA::GameConfig& config)
		: config(&config), runner(createGameRunner(config)), gameBattleCount(0)
	{
	}

	int Arena::runGames(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, int playerCount, int seed, int gamesNumber, int mapNumber)
	{
		game_results.clear();

		currentMapID = 0;
		currentSeed = seed;

		for (int i = 0; i < gamesNumber * mapNumber; ++i)
		{
			gameCount = i;
			gameBattleCount = 0;

			//Change map after playing all gamesNumber in each map
			if (i % gamesNumber == 0 && i != 0)
			{
				currentMapID++;
			}

			currentSeed = seed + i;
			std::mt19937 rngEngine(currentSeed);

			CallbackFn callback = [&](const std::vector<int>& c) {runGame(agentGenerator, c, rngEngine); };
			generateCombinations(config->agentParams.size(), playerCount, callback);
		}

		int total_value = 0;
		for (int n : game_results)
			total_value += n;

		return total_value;
	}

	void Arena::runGame(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, const std::vector<int>& agentAssignment, std::mt19937 rngEngine)
	{
		// Initialize new Game
		//std::cout << "Initializing new game" << std::endl;
		runner->reset(currentMapID);

		// Assign agents
		std::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
		auto allAgents = agentGenerator();


		std::vector<std::unique_ptr<SGA::Agent>> agents(agentAssignment.size());
		std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
		for (size_t i = 0; i < agentAssignment.size(); i++)
		{
			//std::cout << "Player " << i << " is controlled by " << config->agentParams[agentAssignment[i]].first << std::endl;
			agents[i] = std::move(allAgents[agentAssignment[i]]);
			// Set seed of the agents for deterministic behaviour
			agents[i]->setSeed(seedDist(rngEngine));
		}

		// Run the game and return result
		runner->run(agents, this);

		//std::cout << "The winner is: " << runner->getGameState().winnerPlayerID << std::endl;
		// rate result
		int winnerID = runner->getGameState().winnerPlayerID;
		if (agentAssignment[0] == winnerID)
			game_results.push_back(3);
		else if (winnerID == -1)
			game_results.push_back(1);
		else 
			game_results.push_back(0);

	}


	void Arena::onGameStateAdvanced(const SGA::GameState& state, const SGA::EntityForwardModel& forwardModel)
	{

	}

	void Arena::onGameFinished(const SGA::GameState& finalState, const SGA::EntityForwardModel& forwardModel)
	{

	}

}