#include <Configuration/GameConfig.h>
#include <Logging/Log.h>

#include <GameRunner.h>
#include <InputParser.h>

#include <filesystem>


int main(int argc, char** argv)
{
	InputParser parser(argc, argv);
	auto seed = parser.getCmdOption<int>("-seed", 0);
	auto playerCount = parser.getCmdOption<int>("-playerCount", 2);
	auto logPath = parser.getCmdOption<std::string>("-logPath", "./sgaLog.yaml");
	auto configPath = parser.getCmdOption("-configPath");
	if(configPath.empty())
	{
		std::cout << "You have to provide the argument -configPath" << std::endl;
		return 0;
	}
	
	// Read Config
	auto yamlConfig = YAML::LoadFile(configPath);
	auto gameConfig = yamlConfig.as<SGA::GameConfig>();

	// Run games
	SGA::Log::setDefaultLogger(std::make_unique<SGA::FileLogger>(logPath));
	GameRunner runner(gameConfig);
	runner.runGames(playerCount, seed);
	
	return 0;
}
