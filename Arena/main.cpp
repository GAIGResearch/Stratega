#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Logging/Log.h>

#include <Arena.h>
#include <InputParser.h>

#include <filesystem>


int main(int argc, char** argv)
{
	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<unsigned int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/TBS/KillTheKing.yaml");
	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if(configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	
	// Read Config
	auto gameConfig = SGA::loadConfigFromYAML(configPath);

	// Run games
	SGA::Log::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
	Arena arena(*gameConfig);
	arena.runGames(playerCount, seed);
	
	return 0;
}
