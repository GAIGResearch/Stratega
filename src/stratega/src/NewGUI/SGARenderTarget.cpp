#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/World.h>
namespace SGA
{
	SGARenderTarget::SGARenderTarget(sf::RenderTarget& newTarget, const ResourceManager& resourceManager, const World& world) :
		target(newTarget), resourceManager(resourceManager), world(world)
	{

	}

	void SGARenderTarget::drawTile(const Vector2i& position, const TileType& tile)
	{
		 auto spriteData=resourceManager.getTileSprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256/2, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 target.draw(sprite);
		 /*sf::Texture& texture = assetCache.getTexture("selected");*/
		 //sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
		 ///*sf::Sprite selectedTile(texture);

		 //selectedTile.setPosition(toISO(mouseGridPos.x, mouseGridPos.y));
		 //selectedTile.setOrigin(origin);*/
		 //window.draw(selectedTile);
	}

	void SGARenderTarget::drawVertices(sf::VertexArray vertexArray)
	{
		target.draw(vertexArray);
	}
}