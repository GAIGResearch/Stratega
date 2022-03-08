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
	
	

	static std::string TILE_SELECTION_SPRITE_NAME; // This name is unlikely to be occupied by something else
	class ResourceManager
	{
	public:
		ResourceManager();

		const sf::Font& getDefaultFont() const;
		sf::Shader& getOutlineShader();
		sf::Color getPlayerColor(int playerID) const;
		
		sf::Vector2u getSpriteSize() const;
		sf::Vector2u getTileSpriteSize() const;
		bool tryGetTileSelectionSprite(SpriteData& outputData) const;
		bool tryGetFowSprite(SpriteData& outputData) const;
		SpriteData getEntitySprite(const EntityType& entityType) const;
		SpriteData getTileSprite(const TileType& entityType) const;
		SpriteData getTileSprite(const std::string& spriteName) const;
		SpriteData getEntitySprite(const std::string& spriteName) const;
		const TextureAtlas& getTileAtlas() const 
		{
			return tileAtlas;
		}
		const TextureAtlas& getEntityAtlas() const
		{
			return entityAtlas;
		}

		SpriteData getSprite(const std::string& spriteName) const;
		
		static std::unique_ptr<ResourceManager> constructFromConfig(const GameConfig& config);
	
	private:
		 

		TextureAtlas entityAtlas;
		TextureAtlas tileAtlas;
		sf::Font defaultTextFont;
		sf::Shader outlineShader;

		//// The 12 colors used in Warcraft 3
		std::vector<sf::Color> DEFAULT_PLAYER_COLORS;
	};
}