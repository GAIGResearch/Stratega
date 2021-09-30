#include <Stratega//Configuration/GameConfigParser.h>
#include <Stratega/Game/GameRunner.h>
#include <Stratega/Utils/filesystem.hpp>
#undef max
int main()
{
	////Check if config exist	
	//std::string temp = "D:\\Programas\\Python\\lib\\site-packages\\stratega.cp39-win_amd64.pyd";
	//std::cout << "Beforeeee " << temp << std::endl;
	////std::replace(temp.begin(), temp.end(), '\\', '/');
	////ghc::filesystem::path::generic_string(temp);
	//ghc::filesystem::path newPossiblePath(temp);
	//
	//std::cout << "Beforeeee1 " << temp << std::endl;
	//std::cout << "Beforeeee2 " << newPossiblePath << std::endl;
	////newPossiblePath.make_preferred();
	//std::cout << "Beforeeee3 " << newPossiblePath << std::endl;
	//ghc::filesystem::path path = newPossiblePath.parent_path() / "resources/gameConfigurations/TBS/KillTheKing.yaml";
	//std::cout << "After1 " << path << std::endl;
	//path.make_preferred();
	//std::cout << "After2 " << path << std::endl;
	//std::cout << "Checking if file exist in module: " << path << std::endl;

	std::mt19937 rngEngine(0);
	std::string configPath("../../../resources/gameConfigurations/TBS/KillTheKing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	
	/*SGA::Vector2f resolution;
	if (gameConfig->gameType == SGA::GameType::TBS)
	{
		resolution = SGA::Vector2f(800, 600);
	}
	else
	{
		resolution = SGA::Vector2f(1200, 800);
	}*/

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
	gameRunner->play(agents.begin(), agents.end(), gameConfig->renderConfig->resolution);
	
    return 0;
}
