#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/NewGUI/ResourceManager.h>
#include <Stratega/NewGUI/SpriteData.h>
#include <Stratega/Representation/EntityType.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/TileType.h>

namespace SGA {
ResourceManager::ResourceManager() : spriteAtlas(), defaultTextFont() 
{
    TILE_SELECTION_SPRITE_NAME = "@_selectionSprite_@";
    DEFAULT_PLAYER_COLORS = {
            sf::Color::Red,
            sf::Color::Blue,
            sf::Color{ 0, 128, 128 }, // Teal
            sf::Color{ 128,0,128 }, // Purple
            sf::Color::Yellow,
            sf::Color{ 255,165,0 }, // Orange
            sf::Color::Green,
            sf::Color{ 255,192,203 }, // Pink
            sf::Color{ 128,128,128 }, // Grey
            sf::Color{ 173,216,230 }, // Light Blue
            sf::Color{ 0,100,0 }, // Dark Green
            sf::Color{ 139,69,19 } // Brown
    };
}

const sf::Font& ResourceManager::getDefaultFont() const
{
   return defaultTextFont;
}

const sf::Shader& ResourceManager::getOutlineShader() const
{
   return outlineShader;
}

sf::Color ResourceManager::getPlayerColor(int playerID) const
{
   if(playerID == Player::getNeutralPlayerID()) {
      return sf::Color::Transparent;
   }
   if(playerID >= 0 && playerID < static_cast< int >(DEFAULT_PLAYER_COLORS.size())) {
      return DEFAULT_PLAYER_COLORS.at(playerID);
   }

   throw std::runtime_error(
      "Tried accessing the color for the player with id " + std::to_string(playerID)
      + " which is not defined");
}

sf::Vector2u ResourceManager::getSpriteSize() const
{
   return spriteAtlas.getSpriteSize();
}

bool ResourceManager::tryGetTileSelectionSprite(SpriteData& outputData) const
{
   if(spriteAtlas.isDefined(TILE_SELECTION_SPRITE_NAME)) {
      outputData.setTexture(spriteAtlas.getAtlasTexture());
      outputData.setSpriteRect(spriteAtlas.getSpriteRect(TILE_SELECTION_SPRITE_NAME));
      return true;
   }
   return false;
}

bool ResourceManager::tryGetFowSprite(SpriteData& outputData) const
{
   //if(spriteAtlas.isDefined(TileType::FOG_OF_WAR_TYPE.getName())) {
   //   outputData.setTexture(spriteAtlas.getAtlasTexture());
   //   outputData.setSpriteRect(spriteAtlas.getSpriteRect(TileType::FOG_OF_WAR_TYPE.getName()));
   //   return true;
   //}
   return false;
}

SpriteData ResourceManager::getEntitySprite(const EntityType& entityType) const
{
   return getSprite(entityType.getName());
}

SpriteData ResourceManager::getTileSprite(const TileType& entityType) const
{
   return getSprite(entityType.getName());
}

SpriteData ResourceManager::getSprite(const std::string& spriteName) const
{
   return SpriteData{spriteAtlas.getAtlasTexture(), spriteAtlas.getSpriteRect(spriteName)};
}

std::unique_ptr< ResourceManager > ResourceManager::constructFromConfig(const GameConfig& config)
{
   auto manager = std::unique_ptr< ResourceManager >(new ResourceManager());
   manager->defaultTextFont.loadFromFile("./GUI/Assets/arial.ttf");

   // Construct texture atlas
   auto spriteCount = config.renderConfig->entitySpritePaths.size();
   spriteCount += config.renderConfig->tileSpritePaths.size();
   spriteCount += ! config.renderConfig->selectedPath.empty();

   sf::Image image;
   image.loadFromFile(config.renderConfig->tileSpritePaths.begin()->second);
   manager->spriteAtlas.create(image.getSize(), spriteCount, 4);

   // Fill atlas
   for(const auto& namePathPair : config.renderConfig->tileSpritePaths) {
      image.loadFromFile(namePathPair.second);
      manager->spriteAtlas.emplace(namePathPair.first, image);
   }
   for(const auto& namePathPair : config.renderConfig->entitySpritePaths) {
      image.loadFromFile(namePathPair.second);
      manager->spriteAtlas.emplace(namePathPair.first, image);
   }

   if(! config.renderConfig->selectedPath.empty()) {
      image.loadFromFile(config.renderConfig->selectedPath);
      manager->spriteAtlas.emplace(TILE_SELECTION_SPRITE_NAME, image);
   }
   //if(config.renderConfig->.has_value()) {
   //   image.loadFromFile(config.renderConfig->fogOfWarSpritePath.value());
   //   manager->spriteAtlas.emplace(TileType::FOG_OF_WAR_TYPE.getName(), image);
   //}

   // Initialise shader
   manager->outlineShader.loadFromFile(
      config.renderConfig->outlineShaderPath, sf::Shader::Fragment);
   manager->outlineShader.setUniform("texture", sf::Shader::CurrentTexture);
   manager->outlineShader.setUniform("outlineThickness", 4.f);
   manager->outlineShader.setUniform(
      "textureSize", sf::Glsl::Vec2(manager->spriteAtlas.getAtlasTexture().getSize()));

   return manager;
}
}  // namespace SGA
