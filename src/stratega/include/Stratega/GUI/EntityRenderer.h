#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <Stratega/GUI/TextureAtlas.h>

namespace SGA
{
	struct GameConfig;
	struct RenderConfig;
	struct GameState;

	// The 12 colors used in Warcraft 3
	static const std::vector<sf::Color> PLAYER_COLORS = {
		sf::Color::Red,
		sf::Color::Blue,
		sf::Color{0, 128, 128}, // Teal
		sf::Color{128,0,128}, // Purple
		sf::Color::Yellow,
		sf::Color{255,165,0}, // Orange
		sf::Color::Green,
		sf::Color{255,192,203}, // Pink
		sf::Color{128,128,128}, // Grey
		sf::Color{173,216,230}, // Light Blue
		sf::Color{0,100,0}, // Dark Green
		sf::Color{139,69,19} // Brown
	};

	class EntityRenderer : public sf::Drawable, public sf::Transformable
	{
	public:	

		EntityRenderer();
		
		void init(const GameState& initialState, const GameConfig& gameConfig, const RenderConfig& renderConfig);
		void update(const GameState& state);

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		sf::VertexArray vertices;
		TextureAtlas atlas;
		std::unordered_map<int, std::string> entitySpritePaths;
		sf::Shader outlineShader;
	};
}
