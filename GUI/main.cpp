#include <Stratega/Configuration/GameConfigParser.h>
#include <Stratega/Game/TBSGameRunner.h>

int main()
{
	std::string configPath("../../../gameConfigs/TBS/CityCapturing.yaml");
	auto gameConfig = SGA::loadConfigFromYAML(configPath);
	
	auto agents = gameConfig->generateAgents();
	SGA::TBSGameRunner gameRunner(std::move(gameConfig));
	gameRunner.play(agents);
	
    return 0;
}
