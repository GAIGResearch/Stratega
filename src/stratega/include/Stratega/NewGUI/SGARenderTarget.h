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
	struct RenderConfig;
	class SGARenderTarget
	{
	public:
		SGARenderTarget(sf::RenderTarget& newTarget, ResourceManager& newResourceManager, const World& newWorld, const RenderConfig& newRenderConfig);

		void drawTile(const Vector2f& position, const TileType& type, float alpha = 1.0f);
		void drawEntity(const Vector2f& position, const EntityType& type, float alpha = 1.0f);

		void drawTile(const Vector2f& position, const std::string& spriteName, float alpha = 1.0f);
		void drawEntity(const Vector2f& position, const std::string& spriteName, float alpha = 1.0f);

		void drawTileHighlight(const Vector2f& position, const TileType& type, float alpha = 1.0f);
		void drawEntityHighlight(const Vector2f& position, const EntityType& type, float alpha = 1.0f);

		void drawTileHighlight(const Vector2f& position, const std::string& spriteName, float alpha = 1.0f);
		void drawEntityHighlight(const Vector2f& position, const std::string& spriteName, float alpha = 1.0f);

		void drawTileOutlineColor(const Vector2f& position, const TileType& type, sf::Color color, float alpha = 1.0f);
		void drawEntityOutlineColor(const Vector2f& position, const EntityType& type, sf::Color color, float alpha = 1.0f);

		void drawTileOutlineColor(const Vector2f& position, const std::string& spriteName, sf::Color color, float alpha = 1.0f);
		void drawEntityOutlineColor(const Vector2f& position, const std::string& spriteName, sf::Color color,float alpha = 1.0f);

		void drawVertices(sf::VertexArray vertexArray);

		const ResourceManager& getResourceManager()
		{
			return resourceManager;
		}
		const RenderConfig& getRenderConfig()
		{
			return renderConfig;
		}
	  private:

	   sf::RenderTarget& target;
	   ResourceManager& resourceManager;
	   const World& world;
	   const RenderConfig& renderConfig;
	};
}