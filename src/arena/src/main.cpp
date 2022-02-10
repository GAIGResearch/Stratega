#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Logging/Log.h>
#include <Stratega/Arena/Arena.h>
#include <Stratega/Arena/InputParser.h>

int main(int argc, char** argv)
{
	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<unsigned int>("-seed", 13837);
	auto numberOfGames = parser.getCmdOption<unsigned int>("-gamesNumber", 1);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	//Currently obsolete but configPath shouldn't have a default value. So we keep it until then
    auto configPath = parser.getCmdOption< std::string >("-configPath", "../resources/gameConfigurations/TBS/KillTheKing.yaml");
	//Optional
	auto mapsPath = parser.getCmdOption<std::string>("-mapsPath", "../resources/gameConfigurations/TBS/KtKMaps_kwah.yaml");
	if(configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}	
	
	// Read Config
	auto gameConfig = SGA::loadConfigFromYAML(configPath);

	// Run games
	SGA::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
	Arena arena(*gameConfig);
	if(mapsPath.empty())
	{
		//Run single map defined in the game config
		arena.runGames(playerCount, seed, numberOfGames);
	}
	else
	{
		//Load level definitions and exchange them in the game config
		gameConfig->levelDefinitions = SGA::loadLevelsFromYAML(mapsPath, *gameConfig);
		//Run combinations per map
		const int mapNumber = static_cast<int>(gameConfig->levelDefinitions.size());
		arena.runGames(playerCount, seed, numberOfGames, mapNumber);
	}	
	
	return 0;
}
