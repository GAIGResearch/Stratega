#include <Stratega/GUI/TextureAtlas.h>
#include <math.h>
namespace SGA
{
	TextureAtlas::TextureAtlas()
		: spriteSize(), spriteCount(), atlas(), pixelGap()
	{
	}

	void TextureAtlas::create(sf::Vector2u newSpriteSize, size_t numSprites, int newPixelGap)
	{
		this->spriteSize = newSpriteSize;
		this->pixelGap = newPixelGap;

		// Compute optimal number of sprites in the x-axis and y-axis to form a perfect square
		auto ratio = newSpriteSize.y / static_cast<double>(newSpriteSize.x);
		auto xCountF = sqrt(static_cast<double>(numSprites) / ratio);
		auto yCountF = xCountF * ratio;

		// Since we don't want to split up a texture, turn the square into a slightly inaccurate rectangle
		spriteCount.x = static_cast<unsigned int>(ceil(xCountF));
		spriteCount.y = static_cast<unsigned int>(ceil(yCountF));
		assert(spriteCount.x * spriteCount.y >= numSprites);

		// Create atlas
		auto width = (newSpriteSize.x * spriteCount.x + (newSpriteSize.x - static_cast<unsigned int>(1)) * static_cast<unsigned int>(newPixelGap));
		auto height = (newSpriteSize.y * spriteCount.y + (newSpriteSize.y - static_cast<unsigned int>(1)) * static_cast<unsigned int>(newPixelGap));
		atlas.create(width, height);
	}

	void TextureAtlas::emplace(const std::string& key, const sf::Image& image)
	{
		if (currentPosition == spriteCount)
		{
			throw std::out_of_range("Exceeded the number of sprites that can be inserted into the texture atlas");
		}
		if (image.getSize() != spriteSize)
		{
			auto expectedDim = "(" + std::to_string(spriteSize.x) + "," + std::to_string(spriteSize.y) + ")";
			auto givenDim = "(" + std::to_string(image.getSize().x) + "," + std::to_string(image.getSize().y) + ")";
			throw std::invalid_argument("Tried adding a sprite to texture-atlas with dimension " + givenDim + " expected " + expectedDim);
		}

		// Insert the image into the atlas
		auto startX = currentPosition.x * (spriteSize.x + static_cast<unsigned int>(pixelGap));
		auto startY = currentPosition.y * (spriteSize.y + static_cast<unsigned int>(pixelGap));
		atlas.update(image, startX, startY);
		rectLookup.emplace(key, sf::IntRect(static_cast<int>(startX), static_cast<int>(startY), static_cast<int>(spriteSize.x), static_cast<int>(spriteSize.y)));

		// Update position
		currentPosition.x++;
		if (currentPosition.x == spriteCount.x)
		{
			currentPosition.x = 0;
			currentPosition.y++;
		}
	}

	bool TextureAtlas::isDefined(const std::string& key) const
	{
		return rectLookup.find(key) != rectLookup.end();
	}

}