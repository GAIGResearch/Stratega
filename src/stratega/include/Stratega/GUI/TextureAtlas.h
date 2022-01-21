#pragma once
#include <cassert>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

namespace SGA
{
	class TextureAtlas
	{
		sf::Vector2u spriteSize;
		sf::Vector2u currentPosition;
		sf::Vector2u spriteCount;
		sf::Texture atlas;
		int pixelGap;

		std::unordered_map<std::string, sf::IntRect> rectLookup;

	public:
		TextureAtlas();

		void create(sf::Vector2u newSpriteSize, size_t numSprites, int newPixelGap = 0);
		void emplace(const std::string& key, const sf::Image& image);
		bool isDefined(const std::string& key) const;

		const sf::Texture& getAtlasTexture() const { return atlas; }
		sf::IntRect getSpriteRect(const std::string& spritePath) const { return rectLookup.at(spritePath); }
		const sf::Vector2u& getSpriteSize() const { return spriteSize; }
	};
}