#include <Stratega/NewGUI/SpriteData.h>

namespace SGA
{
	SpriteData::SpriteData()
		: spriteTexture(nullptr), spriteRect()
	{
	}

	
	SpriteData::SpriteData(const sf::Texture& texture, const sf::IntRect& rect, const sf::Vector2u& size)
		: spriteTexture(&texture), spriteRect(rect), spriteSize(size)
	{
	}

	const sf::Texture* SpriteData::getTexture() const
	{
		return spriteTexture;
	}

	void SpriteData::setTexture(const sf::Texture& texture)
	{
		spriteTexture = &texture;
	}

	const sf::IntRect& SpriteData::getSpriteRect() const
	{
		return spriteRect;
	}

	const sf::Vector2u& SpriteData::getSpriteSize() const
	{
		return spriteSize;
	}

	const sf::FloatRect SpriteData::getFloatSpriteRect() const
	{
		return sf::FloatRect(static_cast<float>(spriteRect.left), static_cast<float>(spriteRect.top), static_cast<float>(spriteRect.width), static_cast<float>(spriteRect.height));
	}


	void SpriteData::setSpriteRect(sf::IntRect rect)
	{
		spriteRect = rect;
	}


	sf::Sprite SpriteData::createSprite() const
	{
		return sf::Sprite(*spriteTexture, spriteRect);
	}
}