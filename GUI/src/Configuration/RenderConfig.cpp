#include <Configuration/RenderConfig.h>
#include <Configuration/GameConfig.h>

namespace SGA
{
	std::unique_ptr<GameStateRenderBase> stateRendererFromConfig(Game& game, const RenderConfig& config, const GameConfig& gameConfig, int playerID)
	{
		std::unordered_map<int, std::string> tilePaths;
		for(const auto& namePathPair : config.tileSpritePaths)
		{
			int tileID = gameConfig.getTileID(namePathPair.first);
			tilePaths.emplace(tileID, namePathPair.second);
		}
		
		if(gameConfig.gameType == ForwardModelType::TBS)
			return std::make_unique<TBSGameStateRender>(dynamic_cast<TBSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
		else if(gameConfig.gameType == ForwardModelType::RTS)
			return std::make_unique<RTSGameStateRender>(dynamic_cast<RTSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
		else
			throw std::runtime_error("Tried generating a GameStateRender with unknown game-type");
	}
}
