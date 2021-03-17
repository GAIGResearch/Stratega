#include <Configuration/RenderConfig.h>
#include <Stratega/Configuration/GameConfig.h>

namespace SGA
{
	std::unique_ptr<GameStateRenderBase> stateRendererFromConfig(Game& game, const RenderConfig& config, const GameConfig& gameConfig, int playerID)
	{
		if(gameConfig.gameType == GameType::TBS)
		{
			std::unique_ptr<TBSGameStateRender>stateRenderer=std::make_unique<TBSGameStateRender>(dynamic_cast<TBSGame&>(game), gameConfig, config, playerID);
			
			return std::move(stateRenderer);
		}
		else if(gameConfig.gameType == GameType::RTS)
		{
			std::unique_ptr<RTSGameStateRender>stateRenderer = std::make_unique<RTSGameStateRender>(dynamic_cast<RTSGame&>(game), gameConfig, config, playerID);
			
			return std::move(stateRenderer);
		}
		else
			throw std::runtime_error("Tried generating a GameStateRender with unknown game-type");
	}
}
