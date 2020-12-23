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
		
		if(gameConfig.gameType=="TBS")
			return std::make_unique<AbstractTBSGameStateRender>(dynamic_cast<AbstractTBSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
		else if(gameConfig.gameType == "RTS")
			return std::make_unique<AbstractRTSGameStateRender>(dynamic_cast<AbstractRTSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
		else
			throw std::runtime_error("Tried generating a GameStateRender with unknown game-type " + gameConfig.gameType);
	}
}
