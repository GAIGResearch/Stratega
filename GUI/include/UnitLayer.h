#pragma once
#include <RenderLayer.h>

#include <Representation/RTSGameState.h>
#include <Representation/TBSGameState.h>

class TBSUnitLayer : public RenderLayer<SGA::TBSGameState>
{
public:
	TBSUnitLayer(AssetCache& assetCache);
	void init(SGA::TBSGameState& state) override;
	void update(SGA::TBSGameState& state) override;
	void draw(SGA::TBSGameState& state, sf::RenderWindow& window)const override;
private:
	std::vector<sf::Text> unitsInfo;
};

class RTSUnitLayer : public RenderLayer<SGA::RTSGameState>
{
public:
	RTSUnitLayer(AssetCache& assetCache);
	void init(SGA::RTSGameState& state) override;
	void update(SGA::RTSGameState& state) override;
	void draw(SGA::RTSGameState& state, sf::RenderWindow& window)const override;
private:
	std::vector<sf::Text> unitsInfo;
};