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
		
		//Shader stuff
		//Fragment sourceFile
		std::string shaderFrag = "../GUI/Assets/OutLine.frag";

		
		
		if(gameConfig.gameType == ForwardModelType::TBS)
		{
			std::unique_ptr<TBSGameStateRender>stateRenderer=std::make_unique<TBSGameStateRender>(dynamic_cast<TBSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
			//Load fragment
			if (!stateRenderer->outLineShadeR.loadFromFile(shaderFrag, sf::Shader::Fragment))
				std::cout << "Error: Could not load outlineShader" << std::endl;

			//Set parameters
			stateRenderer->outLineShadeR.setUniform("targetCol", sf::Glsl::Vec4(0.7, 0.1, 0.2, 1.0));
			stateRenderer->outLineShadeR.setUniform("texture", sf::Shader::CurrentTexture);
			return std::move(stateRenderer);
		}
		else if(gameConfig.gameType == ForwardModelType::RTS)
		{
			std::unique_ptr<RTSGameStateRender>stateRenderer = std::make_unique<RTSGameStateRender>(dynamic_cast<RTSGame&>(game), tilePaths, config.entitySpritePaths, playerID);
			//Load fragment
			if (!stateRenderer->outLineShadeR.loadFromFile(shaderFrag, sf::Shader::Fragment))
				std::cout << "Error: Could not load outlineShader" << std::endl;

			//Set parameters
			stateRenderer->outLineShadeR.setUniform("targetCol", sf::Glsl::Vec4(0.0, 191.0, 255.0, 1.0));
			stateRenderer->outLineShadeR.setUniform("texture", sf::Shader::CurrentTexture);
			return std::move(stateRenderer);
		}
		else
			throw std::runtime_error("Tried generating a GameStateRender with unknown game-type");
	}
}
