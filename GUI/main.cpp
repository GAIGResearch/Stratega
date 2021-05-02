#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Game/TBSGameRunner.h>

int main()
{
	std::mt19937 rngEngine(0);
	std::string configPath("../../../gameConfigs/TBS/CityCapturing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	
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
	
	SGA::TBSGameRunner gameRunner(*gameConfig);
	gameRunner.run(agents);
	
    return 0;
}
