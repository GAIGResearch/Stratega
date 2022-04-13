#include <Stratega//Configuration/GameConfigParser.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max
#include <Stratega/Utils/cparse/shunting-yard.h>
#include <Stratega/Utils/cparse/builtin-features.h>

int main()
{
	cparse_startup();

	boost::mt19937 rngEngine(0);
	//std::string configPath("../resources/gameConfigurations/TBS/TheBattleOfStratega.yaml");
	//std::string configPath("../resources/gameConfigurations/RTS/Original/Settlers.yaml");
	//std::string configPath("../resources/gameConfigurations/TBS/Tests/KillTheKingQueueTest.yaml");
	std::string configPath("../resources/gameConfigurations/TBS/Original/KillTheKing.yaml");
	//std::string configPath("../resources/gameConfigurations/TBS/Ported/Pathway.yaml");
	//std::string configPath("../resources/gameConfigurations/TBS/Ported/IntoTheBreach.yaml");
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
