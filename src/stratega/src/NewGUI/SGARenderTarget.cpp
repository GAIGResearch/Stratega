#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/World.h>
namespace SGA
{
	SGARenderTarget::SGARenderTarget(sf::RenderTarget& newTarget, const ResourceManager& resourceManager, const World& world) :
		target(newTarget), resourceManager(resourceManager), world(world)
	{

	}

	void SGARenderTarget::drawTile(const Vector2f& position, const TileType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256/2, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 auto color = sprite.getColor();
		 color.a = alpha;
		 sprite.setColor(color);
		 target.draw(sprite);
	}
	
	void SGARenderTarget::drawEntity(const Vector2f& position, const EntityType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(spriteData.getSpriteSize().x/2, 360);
		 sprite.setOrigin(origin);

		 auto color = sprite.getColor();
		 color.a = alpha;
		 sprite.setColor(color);
		 //sf::Vector2f origin(UNIT_OFFSET_ORIGIN_X, UNIT_OFFSET_ORIGIN_Y);
		 target.draw(sprite);
	}

	void SGARenderTarget::drawTile(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256/2, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 auto color = sprite.getColor();
		 color.a = alpha;
		 sprite.setColor(color);
		 target.draw(sprite);
	}
	
	void SGARenderTarget::drawEntity(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(spriteData.getSpriteSize().x/2, 360);
		 sprite.setOrigin(origin);

		 auto color = sprite.getColor();
		 color.a = alpha;
		 sprite.setColor(color);
		 //sf::Vector2f origin(UNIT_OFFSET_ORIGIN_X, UNIT_OFFSET_ORIGIN_Y);
		 target.draw(sprite);
	}

	void SGARenderTarget::drawVertices(sf::VertexArray vertexArray)
	{
		target.draw(vertexArray);
	}
}