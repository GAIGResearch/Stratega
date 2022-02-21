#include <Stratega//Configuration/GameConfigParser.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max
#include <Stratega/Utils/cparse/shunting-yard.h>
#include <Stratega/Utils/cparse/builtin-features.h>

int main()
{
	cparse_startup();

	cparse::TokenMap vars;
	vars["pi"] = 3.14;
	//std::cout << cparse::calculator::calculate("-pi+1", vars) << std::endl; // 2.14
	//std::cout << cparse::calculator::calculate("pi + 1", vars) << std::endl; // 0.14
	//std::cout << cparse::calculator::calculate("pi-b == 3", vars) << std::endl; // 1 (true)
	//std::cout << cparse::calculator::calculate("sqrt(4) + 3**2 == 11") << std::endl; // 1 (true)

	std::mt19937 rngEngine(0);
	//std::string configPath("../resources/gameConfigurations/RTS/BasicRTS.yaml");
	std::string configPath("../resources/gameConfigurations/TBS/KillTheKing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);

	std::string expressionTest = "Health*2";
	auto entity = gameConfig->entityTypes[0].instantiateEntity(0);
	for (auto& parameter : gameConfig->entityTypes[0].getParameters())
	{
		vars[parameter.second.getName()] = parameter.second.getDefaultValue();
	}
	std::cout << cparse::calculator::calculate(expressionTest.c_str(), vars) << std::endl;

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
