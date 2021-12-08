//#include <Stratega/NewGUI/TextureAtlas.h>
//
//namespace SGA
//{
//	TextureAtlas::TextureAtlas()
//		: spriteSize(), spriteCount(), atlas(), pixelGap()
//	{
//	}
//
//	void TextureAtlas::create(sf::Vector2u spriteSize, size_t numSprites, int pixelGap)
//	{
//		this->spriteSize = spriteSize;
//		this->pixelGap = pixelGap;
//
//		// Compute optimal number of sprites in the x-axis and y-axis to form a perfect square
//		auto ratio = spriteSize.y / static_cast<double>(spriteSize.x);
//		auto xCountF = std::sqrt(numSprites / ratio);
//		auto yCountF = xCountF * ratio;
//
//		// Since we don't want to split up a texture, turn the square into a slightly inaccurate rectangle
//		spriteCount.x = static_cast<unsigned int>(std::ceil(xCountF));
//		spriteCount.y = static_cast<unsigned int>(std::ceil(yCountF));
//		assert(spriteCount.x * spriteCount.y >= numSprites);
//
//		// Create atlas
//		auto width = spriteSize.x * spriteCount.x + (spriteSize.x - 1) * pixelGap;
//		auto height = spriteSize.y * spriteCount.y + (spriteSize.y - 1) * pixelGap;
//		atlas.create(width, height);
//	}
//
//	void TextureAtlas::emplace(const std::string& key, const sf::Image& image)
//	{
//		if (currentPosition == spriteCount)
//		{
//			throw std::out_of_range("Exceeded the number of sprites that can be inserted into the texture atlas");
//		}
//		if (image.getSize() != spriteSize)
//		{
//			auto expectedDim = "(" + std::to_string(spriteSize.x) + "," + std::to_string(spriteSize.y) + ")";
//			auto givenDim = "(" + std::to_string(image.getSize().x) + "," + std::to_string(image.getSize().y) + ")";
//			throw std::invalid_argument("Tried adding a sprite to texture-atlas with dimension " + givenDim + " expected " + expectedDim);
//		}
//
//		// Insert the image into the atlas
//		auto startX = currentPosition.x * (spriteSize.x + pixelGap);
//		auto startY = currentPosition.y * (spriteSize.y + pixelGap);
//		atlas.update(image, startX, startY);
//		rectLookup.emplace(key, sf::IntRect(static_cast<int>(startX), static_cast<int>(startY), static_cast<int>(spriteSize.x), static_cast<int>(spriteSize.y)));
//
//		// Update position
//		currentPosition.x++;
//		if (currentPosition.x == spriteCount.x)
//		{
//			currentPosition.x = 0;
//			currentPosition.y++;
//		}
//	}
//
//	bool TextureAtlas::isDefined(const std::string& key) const
//	{
//		return rectLookup.find(key) != rectLookup.end();
//	}
//
//}