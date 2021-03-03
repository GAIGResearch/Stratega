#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <TextureAtlas.h>

namespace SGA
{
	struct GameConfig;
	struct RenderConfig;
	struct GameState;
	
	class EntityRenderer : public sf::Drawable, public sf::Transformable
	{
	public:
		void init(const GameConfig& gameConfig, const RenderConfig& renderConfig);
		void update(const GameState& state);

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		sf::VertexArray vertices;
		TextureAtlas atlas;
		std::unordered_map<int, std::string> entitySpritePaths;
	};
}
