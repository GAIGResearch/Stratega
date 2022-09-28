#include <Stratega/Agent/AgentFactory.h>
#include <Stratega/Logging/Log.h>
#include <Stratega/Agent/Agent.h>

#include <Stratega/ntbea/Evaluators/Arena.h>
#include <Stratega/ntbea/utils.h>

namespace SGA
{

	Arena::Arena(const SGA::GameConfig& config)
		: config(&config)//, runner(createGameRunner(config)), gameBattleCount(0)
	{

        auto s = config.generateGameState();

        runner = createGameRunner(config);

        gameBattleCount = 0;
	}

	std::vector<int> Arena::runGames(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, int playerCount, int seed, int gamesNumber, int mapNumber)
	{
        // mcts evaluator calls this function
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
            std::cout<< "x";

			currentSeed = seed + i;
			boost::mt19937 rngEngine(currentSeed);

			CallbackFn callback = [&](const std::vector<int>& c) {runGame(agentGenerator, c, rngEngine); };
            generateCombinations(config->agentParams.size(), playerCount, callback);

            /*std::vector<int> combination1 = {0, 1};
            runGame(agentGenerator, combination1 , rngEngine);
            combination1 = {1,0};
            runGame(agentGenerator, combination1, rngEngine);
            */
		}

		int total_value = 0;
		int games_won = 0;
		for (int n : game_results) {
			total_value += n;
			if (n == 3)
				games_won += 1;
		}

		return std::vector<int>{total_value, games_won};
	}

	void Arena::runGame(std::function<std::vector<std::unique_ptr<Agent>>()> agentGenerator, const std::vector<int>& agentAssignment, boost::mt19937 rngEngine)
	{
		// Initialize new Game
		//std::cout << "Initializing new game" << std::endl;
		runner->reset(currentMapID);

		// Assign agents
		boost::random::uniform_int_distribution<unsigned int> distribution(0, std::numeric_limits<unsigned int>::max());
		auto allAgents = agentGenerator();
        //std::cout<<"generated agents"<<std::endl;
        for (auto& agent : allAgents) {
            agent->is_debug = false;
            //agent->is_debug = true;
        }

		std::vector<std::unique_ptr<SGA::Agent>> agents(agentAssignment.size());
		boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
		for (size_t i = 0; i < agentAssignment.size(); i++)
		{
            /*
            if (allAgents[agentAssignment[i]] == nullptr) {
                std::cout<<"null agent!\n";
            }
            else {
                std::cout<<allAgents[agentAssignment[i]]->getName()<<"\n";
            }
            */
			agents[i] = std::move(allAgents[agentAssignment[i]]);
			// Set seed of the agents for deterministic behaviour
			agents[i]->setSeed(seedDist(rngEngine));
		}

		// Run the game and return result
		//runner->run(2, this, 1);
        runner->run(agents.begin(), agents.end(), this);

		//std::cout << "The winner is: " << runner->getGameState().winnerPlayerID << std::endl;
		// rate result
		int winnerID = runner->getGameState().getWinnerID();
		if (agentAssignment[0] == winnerID)
			game_results.push_back(3);
		else if (winnerID == -1)
			game_results.push_back(1);
		else 
			game_results.push_back(0);

	}


	void Arena::onGameStateAdvanced(const SGA::GameState& state, const SGA::ForwardModel& forwardModel)
	{

	}

	void Arena::onGameFinished(const SGA::GameState& finalState, const SGA::ForwardModel& forwardModel)
	{

	}

}