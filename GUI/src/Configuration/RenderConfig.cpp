#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>

namespace SGA
{
	std::unique_ptr<GameStateRenderBase> stateRendererFromConfig(Game& game, const RenderConfig& config, const GameConfig& gameConfig, int playerID)
	{
		std::unordered_map<int, std::string> tilePaths;
		auto tiles = tileTypesFromConfig(gameConfig);
		for(const auto& namePathPair : config.tileSpritePaths)
		{
			tilePaths.emplace(tiles[namePathPair.first].id, namePathPair.second);
		}

		std::unordered_map<int, std::string> unitPaths;
		auto units= unitTypesFromConfig(gameConfig);
		for (const auto& namePathPair : config.unitSpritePaths)
		{
			unitPaths.emplace(units[namePathPair.first].id, namePathPair.second);
		}
		if(gameConfig.gameType=="TBS")
			return std::make_unique<AbstractGameStateRender>(dynamic_cast<AbstractTBSGame&>(game), tilePaths, unitPaths,playerID);
		/*else if(gameConfig.gameType == "RTS")
			return std::make_unique<AbstractGameStateRender>(dynamic_cast<AbstractRTSGame&>(game), tilePaths, unitPaths,playerID);
		*/else
			throw std::runtime_error("Tried generating a GameStateRender with unknown game-type " + gameConfig.gameType);
	}
}
