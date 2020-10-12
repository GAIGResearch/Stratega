#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <AssetCache.h>
#include <Representation/TBSGameState.h>
#include <Representation/Board.h>
#include <GridUtils.h>
template<typename GameState>
class RenderLayer
{
public:
	RenderLayer(AssetCache& assetCache):
		assetCache(assetCache)
	{
	}
	virtual ~RenderLayer() = default;
	
	std::vector<sf::Sprite> sprites;	

	AssetCache& assetCache;
	
	virtual void init(GameState& state) =0;
	virtual void update(GameState& state) =0;
	virtual void draw(GameState& state, sf::RenderWindow& window)const =0;	
};