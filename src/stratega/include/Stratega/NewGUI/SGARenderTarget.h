#pragma once
#include <SFML/Graphics.hpp>

namespace SGA {
class SGARenderTarget {
  public:
   SGARenderTarget(sf::RenderTarget& target, const ResourceManager& resourceManager, const GridLayout& layout);

   // Draw Methods - !!!Do not mark them as const as we might introduce batching at some point!!!
   void drawTile(const TileType& type, const Vector2i& position);
   void drawTile(const Tile& tile);
   void drawTileHighlight(const Vector2i& position);

   void drawEntityCentered(const EntityType& type, const Vector2i& position, const Color& outlineColor = Color::Transparent);
   void drawEntityCentered(const Entity& entity);
   void drawEntity(const EntityType& type, const Vector2f& position, const Color& outlineColor = Color::Transparent);
   void drawEntity(const Entity& entity);

   void drawSprite(const std::string& spriteName, const Vector2f& position);

  private:
   void drawSprite(const SpriteData& data, const Vector2f& position, const Color& outlineColor = Color::Transparent);

   sf::RenderTarget& target;
   const ResourceManager& resourceManager;
   const GridLayout& layout;
};
}