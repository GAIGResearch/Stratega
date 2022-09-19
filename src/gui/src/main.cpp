#include <Stratega//Configuration/GameConfigParser.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max

int main()
{
	boost::mt19937 rngEngine(0);
	//std::string configPath("../resources/gameConfigurations/TBS/PushThemAll.yaml");
    std::string configPath("../resources/gameConfigurations/TBS/KillTheKing_GUI.yaml");
	//std::string configPath("../resources/gameConfigurations/TBS/BasicTBS.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	
	auto agents = gameConfig->generateAgents();
	// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
	boost::random::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
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
	gameRunner->play(agents.begin(), agents.end(), gameConfig->renderConfig->resolution);
	
    return 0;
}
