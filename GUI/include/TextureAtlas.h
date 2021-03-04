#pragma once
#include <cassert>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <SFML/Graphics.hpp>

namespace SGA
{
	class TextureAtlas
	{
		bool initialized;
		sf::Vector2u spriteSize;
		sf::Vector2i spriteCounts;
		sf::Texture atlas;

		sf::Vector2i currentPosition;
		std::unordered_map<std::string, sf::Rect<int>> rectLookup;
		
	public:
		TextureAtlas()
			: initialized(false), spriteSize(), spriteCounts(), atlas(), currentPosition(0, 0)
		{
		}

		void init(const std::vector<std::string>& spritePaths)
		{
			// Initialize data
			sf::Image image;
			if (!image.loadFromFile(spritePaths.at(0)))
			{
				throw std::runtime_error("Could not open file " + spritePaths.at(0));
			}
			spriteSize = image.getSize();
			
			// Compute optimal number of sprites in the x-axis and y-axis to form a perfect square
			auto ratio = spriteSize.y / static_cast<double>(spriteSize.x);
			auto xCountF = std::sqrt(spritePaths.size() / ratio);
			auto yCountF = xCountF * ratio;

			// Since we don't want to split up a texture, turn the square into a slightly inaccurate rectangle
			spriteCounts.x = static_cast<int>(std::ceil(xCountF));
			spriteCounts.y = static_cast<int>(std::ceil(yCountF));
			assert(spriteCounts.x * spriteCounts.y >= spritePaths.size());

			// Create atlas
			atlas.create(spriteSize.x * spriteCounts.x, spriteSize.y * spriteCounts.y);
			for (const auto& path : spritePaths)
			{
				// Load image
				if (!image.loadFromFile(path))
				{
					throw std::runtime_error("Could not open file " + path);
				}
				if (image.getSize() != spriteSize)
				{
					throw std::runtime_error("Textures added to the atlas need to have the same size");
				}

				// Insert the image into the atlas
				auto startX = currentPosition.x * spriteSize.x;
				auto startY = currentPosition.y * spriteSize.y;
				atlas.update(image, startX, startY);
				rectLookup.emplace(path, sf::Rect<int>(startX, startY, spriteSize.x, spriteSize.y));

				// Update position
				currentPosition.x++;
				if (currentPosition.x == spriteCounts.x)
				{
					currentPosition.x = 0;
					currentPosition.y++;
				}
			}

			initialized = true;
		}

		const sf::Texture& getAtlasTexture() const { assert(initialized); return atlas; }
		sf::Rect<int> getSpriteRect(std::string spritePath) const { assert(initialized); return rectLookup.at(spritePath); }
		sf::Vector2u getSpriteSize() const { assert(initialized); return spriteSize; }
	};
}