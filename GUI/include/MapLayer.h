#pragma once
#include <RenderLayer.h>
template<typename GameState>
class MapLayer : public RenderLayer<GameState>
{
public:
	MapLayer(AssetCache& assetCache) :
		RenderLayer<GameState>(assetCache)
	{

	}

	void init(GameState& state) override
	{
		SGA::Board& board = state.getBoard();

		for (int y = 0; y < board.getHeight(); ++y)
		{
			for (int x = 0; x < board.getWidth(); ++x)
			{
				auto& targetTile = board.getTile(x, y);
				sf::Texture& texture = this->assetCache.getTexture("tile_" + std::to_string(targetTile.tileTypeID));
				sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
				sf::Sprite newTile(texture);

				newTile.setPosition(toISO(x, y));
				newTile.setOrigin(origin);
                this->sprites.emplace_back(newTile);
			}

		}
	}

	void update(GameState& state) override
	{

	}

	void draw(GameState& state, sf::RenderWindow& window) const override
	{
		for (const auto& sprite : this->sprites)
		{
			window.draw(sprite);
		}
	}
};
