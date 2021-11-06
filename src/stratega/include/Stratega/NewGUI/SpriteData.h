#pragma once
#include <SFML/Graphics.hpp>

namespace SGA
{
	class SpriteData
	{
	public:
		SpriteData();
		SpriteData(const sf::Texture& texture, const sf::IntRect& rect);
		
		[[nodiscard]] const sf::Texture* getTexture() const;
		void setTexture(const sf::Texture& texture);
		[[nodiscard]] const sf::IntRect& getSpriteRect() const;
		[[nodiscard]] const sf::FloatRect& getFloatSpriteRect() const;
		void setSpriteRect(sf::IntRect rect);
		[[nodiscard]] sf::Sprite createSprite() const;
	
	private:
		const sf::Texture* spriteTexture;
		sf::IntRect spriteRect;
	};
}