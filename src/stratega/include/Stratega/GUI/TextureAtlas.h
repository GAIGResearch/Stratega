#pragma once
#include <cassert>
#include <string>
#include <unordered_map>
//#include <filesystem>
#include <SFML/Graphics.hpp>
#include <cmath>
namespace SGA
{
	class TextureAtlas
	{
		bool initialized;
		sf::Vector2u spriteSize;
		sf::Vector2u spriteCounts;
		sf::Texture atlas;
		int pixelGap;

		std::unordered_map<std::string, sf::Rect<float>> rectLookup;

	public:
		TextureAtlas(int newPixelGap = 0)
			: initialized(false), spriteSize(), spriteCounts(), atlas(), pixelGap(newPixelGap)
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
			auto xCountF = std::sqrt(static_cast<double>(spritePaths.size()) / ratio);
			auto yCountF = xCountF * ratio;

			// Since we don't want to split up a texture, turn the square into a slightly inaccurate rectangle
			spriteCounts.x = static_cast<unsigned int>(std::ceil(xCountF));
			spriteCounts.y = static_cast<unsigned int>(std::ceil(yCountF));
			assert(spriteCounts.x * spriteCounts.y >= spritePaths.size());

			// Create atlas
			auto width = spriteSize.x * spriteCounts.x + (spriteSize.x - 1) * static_cast<unsigned int>(pixelGap);
			auto height = spriteSize.y * spriteCounts.y + (spriteSize.y - 1) *static_cast<unsigned int>( pixelGap);
			atlas.create(width, height);

			// Fill with sprites
			sf::Vector2u currentPosition;
			for (const auto& path : spritePaths)
			{
				// Load image
				if (!image.loadFromFile(path))
				{
					throw std::runtime_error("Could not open file " + path);
				}
				if (image.getSize() != spriteSize)
				{
					throw std::runtime_error("Error loading image " + path + ". Textures added to the atlas need to have the same size: <"
						+ std::to_string(image.getSize().x) + "x" + std::to_string(image.getSize().y) + "> != <"
						+ std::to_string(spriteSize.x) + "x" + std::to_string(spriteSize.y) + ">");
				}

				// Insert the image into the atlas
				auto startX = currentPosition.x * (spriteSize.x + static_cast<unsigned int>(pixelGap));
				auto startY = currentPosition.y * (spriteSize.y + static_cast<unsigned int>(pixelGap));
				atlas.update(image, startX, startY);
				rectLookup.emplace(path, sf::Rect<float>(static_cast<float>(startX), static_cast<float>(startY), static_cast<float>(spriteSize.x), static_cast<float>(spriteSize.y)));

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
		sf::Rect<float> getSpriteRect(const std::string& spritePath) const { assert(initialized); return rectLookup.at(spritePath); }
		sf::Vector2u getSpriteSize() const { assert(initialized); return spriteSize; }
	};
}