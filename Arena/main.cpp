#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Logging/Log.h>

#include <GameRunner.h>
#include <InputParser.h>

#include <filesystem>


int main(int argc, char** argv)
{
	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption<std::string>("-configPath", "../../../gameConfigs/KillTheKing.yaml");
	// Currently obsolete but configPath shouldn't have a default value. So we keep it until then
	if(configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	
	// Read Config
	SGA::GameConfigParser configParser;
	auto gameConfig = configParser.parseFromFile(configPath);

	// Run games
	SGA::Log::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
	GameRunner runner(gameConfig);
	runner.runGames(playerCount, seed);
	
	return 0;
}
