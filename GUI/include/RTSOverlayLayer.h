#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <Representation/RTSGameState.h>
#include <RTSGameStateRender.h>

class RTSGameStateRender;

class RTSOverlayLayer : public RenderLayer<SGA::RTSGameState>
{
public:
	RTSOverlayLayer(AssetCache& assetCache, RTSGameStateRender& renderer, SGA::Vector2f& currentMousePos);
	void init(SGA::RTSGameState& state) override;
	void update(SGA::RTSGameState& state) override;
	void draw(SGA::RTSGameState& state, sf::RenderWindow& window)const override;
private:

	std::vector<sf::Sprite> boxColliderShapes;
	std::vector<sf::Sprite> circleColliderShapes;

	SGA::Vector2f& currentMousePos;

	RTSGameStateRender* renderer;
};