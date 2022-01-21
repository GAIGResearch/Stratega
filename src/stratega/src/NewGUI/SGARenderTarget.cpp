#include <Stratega/NewGUI/SGARenderTarget.h>
#include <Stratega/NewGUI/World.h>

namespace SGA
{
	SGARenderTarget::SGARenderTarget(sf::RenderTarget& newTarget, ResourceManager& resourceManager, const World& world, const RenderConfig& renderConfig) :
		target(newTarget), resourceManager(resourceManager), world(world)
	{
		
	}

	void SGARenderTarget::drawTile(const Vector2f& position, const TileType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256.0f/2.0f, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 auto color = sprite.getColor();
		 color.a = static_cast<sf::Uint8>(alpha);
		 sprite.setColor(color);
		 target.draw(sprite);
	}
	
	void SGARenderTarget::drawEntity(const Vector2f& position, const EntityType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x)/2.0f, 360.0f);
		 sprite.setOrigin(origin);

		 auto color = sprite.getColor();
		 color.a = static_cast<sf::Uint8>(alpha);
		 sprite.setColor(color);
		 target.draw(sprite);
	}

	void SGARenderTarget::drawTile(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256.0f/2.0f, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 auto color = sprite.getColor();
		 color.a = static_cast<sf::Uint8>(alpha);
		 sprite.setColor(color);
		 target.draw(sprite);
	}
	
	void SGARenderTarget::drawEntity(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x) / 2.0f, 360.0f);
		 sprite.setOrigin(origin);
		 auto color = sprite.getColor();
		 color.a = static_cast<sf::Uint8>(alpha);
		 sprite.setColor(color);
		 target.draw(sprite);
	}

	void SGARenderTarget::drawTileHighlight(const Vector2f& position, const TileType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256.0f/2.0f, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);

		 auto color = sf::Color::Yellow;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}
	
	void SGARenderTarget::drawEntityHighlight(const Vector2f& position, const EntityType& tile, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x) / 2.0f, 360.0f);
		 sprite.setOrigin(origin);

		 auto color = sf::Color::Yellow;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture()); 
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}

	void SGARenderTarget::drawTileHighlight(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256.0f/2.0f, static_cast<float>(TILE_ORIGIN_Y));
		 sprite.setOrigin(origin);
		 
		 auto color = sf::Color::Yellow;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}
	
	void SGARenderTarget::drawEntityHighlight(const Vector2f& position, const std::string& spriteName, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x) / 2.0f, 360.0f);
		 sprite.setOrigin(origin);
		 		 
		 auto color = sf::Color::Yellow;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}


	void SGARenderTarget::drawTileOutlineColor(const Vector2f& position, const TileType& tile, sf::Color outlineColor, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256.0f/2.0f, static_cast<float>(TILE_ORIGIN_Y));
		 sprite.setOrigin(origin);

		 auto color = outlineColor;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}
	
	void SGARenderTarget::drawEntityOutlineColor(const Vector2f& position, const EntityType& tile, sf::Color outlineColor, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(tile);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x) / 2.0f, 360.0f);
		 sprite.setOrigin(origin);

		 auto color = outlineColor;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture()); 
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}

	void SGARenderTarget::drawTileOutlineColor(const Vector2f& position, const std::string& spriteName, sf::Color outlineColor, float alpha)
	{
		 auto spriteData=resourceManager.getTileSprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(256/2, TILE_ORIGIN_Y);
		 sprite.setOrigin(origin);
		 
		 auto color = outlineColor;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}
	
	void SGARenderTarget::drawEntityOutlineColor(const Vector2f& position, const std::string& spriteName, sf::Color outlineColor, float alpha)
	{
		 auto spriteData=resourceManager.getEntitySprite(spriteName);
		 auto sprite = spriteData.createSprite();
		 sprite.setPosition(world.toSFML(position));
		 sf::Vector2f origin(static_cast<float>(spriteData.getSpriteSize().x) / 2.0f, 360.0f);
		 sprite.setOrigin(origin);

		 
		 
		 auto color = outlineColor;
		 color.a = static_cast<sf::Uint8>(alpha);
		 auto& outlineShader = resourceManager.getOutlineShader();
		 outlineShader.setUniform("texture", spriteData.getTexture());
		 outlineShader.setParameter("outlineColor", color);
		 outlineShader.setParameter("alpha", alpha);

		 target.draw(sprite, &outlineShader);
	}

	void SGARenderTarget::drawVertices(sf::VertexArray vertexArray)
	{
		target.draw(vertexArray);
	}
}