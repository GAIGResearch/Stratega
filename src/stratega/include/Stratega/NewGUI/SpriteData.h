#pragma once
#include <SFML/Graphics.hpp>

namespace SGA
{
	class SpriteData
	{
	public:
		SpriteData();
		SpriteData(const sf::Texture& texture, const sf::IntRect& rect, const sf::Vector2u&);
		
		const sf::Texture* getTexture() const;
		void setTexture(const sf::Texture& texture);
		const sf::IntRect& getSpriteRect() const;
		const sf::Vector2u& getSpriteSize() const;
		const sf::FloatRect getFloatSpriteRect() const;
		void setSpriteRect(sf::IntRect rect);
		sf::Sprite createSprite() const;
	
	private:
		const sf::Texture* spriteTexture;
		sf::IntRect spriteRect;
		sf::Vector2u spriteSize;
	};
}