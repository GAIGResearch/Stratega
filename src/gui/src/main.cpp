#include <Stratega//Configuration/GameConfigParser.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max
#include <Stratega/Utils/cparse/shunting-yard.h>
#include <Stratega/Utils/cparse/packToken.h>
#include <Stratega/Utils/cparse/functions.h>
#include <Stratega/Utils/cparse/containers.h>

int main()
{
	cparse::TokenMap vars;
	vars["pi"] = 3.14;
	//std::cout << cparse::calculator::calculate("1") << std::endl;

	//std::mt19937 rngEngine(0);
	////std::string configPath("../resources/gameConfigurations/RTS/BasicRTS.yaml");
	//std::string configPath("../resources/gameConfigurations/TBS/KillTheKing.yaml");
	//auto gameConfig = SGA::loadConfigFromYAML(configPath);
	//
	//auto agents = gameConfig->generateAgents();
	//// Set seed of the agents for deterministic behaviour - ToDo Should we move this into Stratega & Should it be done automatically with generateAgents?
	//std::uniform_int_distribution<unsigned int> seedDist(0, std::numeric_limits<unsigned int>::max());
	//for(auto& agent : agents)
	//{
	//	auto seed = seedDist(rngEngine);
	//	// Ignore human player
	//	if(agent != nullptr)
	//	{
	//		agent->setSeed(seed);
	//	}
	//}
	//
	//auto gameRunner = SGA::createGameRunner(*gameConfig);
	//gameRunner->play(agents.begin(), agents.end(), gameConfig->renderConfig->resolution);
	
    return 0;
}
