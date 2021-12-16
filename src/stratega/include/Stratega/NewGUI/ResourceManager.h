#pragma once
#include <Stratega/GUI/TextureAtlas.h>
#include <Stratega/NewGUI/SpriteData.h>
#include <SFML/Graphics.hpp>

namespace SGA
{
	struct Player;
	struct EntityType;
	struct TileType;
	struct Vector2i;
	struct GameConfig;
	
	// The 12 colors used in Warcraft 3
	static const std::vector<sf::Color> DEFAULT_PLAYER_COLORS = 
	{
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

	static std::string TILE_SELECTION_SPRITE_NAME; // This name is unlikely to be occupied by something else
	class ResourceManager
	{
	public:
		ResourceManager();

		const sf::Font& getDefaultFont() const;
		const sf::Shader& getOutlineShader() const;
		sf::Color getPlayerColor(int playerID) const;
		
		sf::Vector2u getSpriteSize() const;
		bool tryGetTileSelectionSprite(SpriteData& outputData) const;
		bool tryGetFowSprite(SpriteData& outputData) const;
		SpriteData getEntitySprite(const EntityType& entityType) const;
		SpriteData getTileSprite(const TileType& entityType) const;
		SpriteData getTileSprite(const std::string& spriteName) const;
		SpriteData getEntitySprite(const std::string& spriteName) const;
		
		static std::unique_ptr<ResourceManager> constructFromConfig(const GameConfig& config);
	
	private:
		 

		TextureAtlas entityAtlas;
		TextureAtlas tileAtlas;
		sf::Font defaultTextFont;
		sf::Shader outlineShader;
	};
}