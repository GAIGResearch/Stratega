#pragma once
#include <RenderLayer.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAction.h>


class TBSOverlayLayer : public RenderLayer<SGA::TBSGameState>
{
public:
	TBSOverlayLayer(AssetCache& assetCache, std::vector<SGA::TBSAction>& actionSpace, SGA::Vector2f& currentMousePos);
	void init(SGA::TBSGameState& state) override;
	void update(SGA::TBSGameState& state) override;
	void draw(SGA::TBSGameState& state, sf::RenderWindow& window)const override;
private:
	std::vector<SGA::TBSAction>& actionSpace;
	std::vector<sf::CircleShape> circleShapes;

	SGA::Vector2f& currentMousePos;
	
};
