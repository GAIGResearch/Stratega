#pragma once
#include <RenderLayer.h>
#include <Representation/RTSGameState.h>
#include <RTSGameStateRender.h>

class RTSGameStateRender;

class RTSUnitLayer : public RenderLayer<SGA::RTSGameState>
{
public:
	RTSUnitLayer(AssetCache& assetCache, RTSGameStateRender& renderer);
	void init(SGA::RTSGameState& state) override;
	void update(SGA::RTSGameState& state) override;
	void draw(SGA::RTSGameState& state, sf::RenderWindow& window)const override;
private:
	std::vector<sf::Text> unitsInfo;
	std::vector<sf::RectangleShape> healthBars;
	RTSGameStateRender* renderer;
};