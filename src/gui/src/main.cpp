#include <stratega/Configuration/GameConfigParser.h>
#include <stratega/Game/GameRunner.h>

int main()
{
	std::mt19937 rngEngine(0);
	std::string configPath("../../../gameConfigs/TBS/KillTheKing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	
	SGA::Vector2f resolution;
	if (gameConfig->gameType == SGA::GameType::TBS)
	{
		resolution = SGA::Vector2f(800, 600);
	}
	else
	{
		resolution = SGA::Vector2f(1200, 800);
	}

	auto agents = gameConfig->generateAgents();
	// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
	std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
	for(auto& agent : agents)
	{
		auto seed = seedDist(rngEngine);
		// Ignore human player
		if(agent != nullptr)
		{
			agent->setSeed(seed);
		}
	}
	
	auto gameRunner = SGA::createGameRunner(*gameConfig);
	gameRunner->play(agents.begin(), agents.end(), resolution);
	
    return 0;
}
