#pragma once
#include <SFML/Graphics.hpp>
#include <Stratega/Representation/Vector2.h>
#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/EntityType.h>
#include <Stratega/Representation/TileType.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/NewGUI/ResourceManager.h>
//#include <Stratega/NewGUI/World.h>
#include <imgui-SFML.h>

namespace SGA {
	struct World;
	class SGARenderTarget
	{
	  public:

		  SGARenderTarget(sf::RenderTarget& newTarget, const ResourceManager& resourceManager, const World& world );

	//	SGARenderTarget(sf::RenderTarget& target, const ResourceManager& resourceManager, World& world)
	//		  target(target), resourceManager(resourceManager), world(world)
	//	{

	//	}

	//  Draw Methods - !!!Do not mark them as const as we might introduce batching at some point!!!
	//  void drawTile(const TileType& type, const Vector2i& position)
	//	{
	//  }

		 void drawTile(const Vector2f& position, const TileType& type, float alpha = 1);
		 void drawEntity(const Vector2f& position, const EntityType& type, float alpha = 1);

		 void drawTile(const Vector2f& position, const std::string& spriteName, float alpha = 1);
		 void drawEntity(const Vector2f& position, const std::string& spriteName, float alpha = 1);

	 //  void drawTileHighlight(const Vector2i& position);

	 //  void drawEntityCentered(const EntityType& type, const Vector2i& position, const Color& outlineColor = Color::Transparent);
	 //  void drawEntityCentered(const Entity& entity);
	 //  void drawEntity(const EntityType& type, const Vector2f& position, const Color& outlineColor = Color::Transparent);
	 //  void drawEntity(const Entity& entity);

		  void drawVertices(sf::VertexArray vertexArray);

		  const ResourceManager& getResourceManager()
		  {
			  return resourceManager;
		  }
	  private:
	 /*  void drawSprite(const SpriteData& data, const Vector2f& position, const Color& outlineColor = Color::Transparent);*/

	   sf::RenderTarget& target;
	   const ResourceManager& resourceManager;
	   const World& world;
	};
}