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
	
	class ResourceManager
	{
	public:
		// The 12 colors used in Warcraft 3
		static const std::vector<sf::Color> DEFAULT_PLAYER_COLORS;

		const sf::Font& getDefaultFont() const;
		const sf::Shader& getOutlineShader() const;
		sf::Color getPlayerColor(int playerID) const;
		
		sf::Vector2u getSpriteSize() const;
		bool tryGetTileSelectionSprite(SpriteData& outputData) const;
		bool tryGetFowSprite(SpriteData& outputData) const;
		SpriteData getEntitySprite(const EntityType& entityType) const;
		SpriteData getTileSprite(const TileType& entityType) const;
		SpriteData getSprite(const std::string& spriteName) const;
		
		static std::unique_ptr<ResourceManager> constructFromConfig(const GameConfig& config);
	
	private:
		static const std::string TILE_SELECTION_SPRITE_NAME; // This name is unlikely to be occupied by something else

		ResourceManager();
		
		TextureAtlas spriteAtlas;
		sf::Font defaultTextFont;
		sf::Shader outlineShader;
	};
}