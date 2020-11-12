#pragma once
#include <RenderLayer.h>
#include <ForwardModel/TBSActionSpace.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <Representation/RTSGameState.h>
#include <Representation/TBSGameState.h>


class TBSOverlayLayer : public RenderLayer<SGA::TBSGameState>
{
public:
	TBSOverlayLayer(AssetCache& assetCache, std::vector<SGA::Action<SGA::Vector2i>>& actionSpace, SGA::Vector2f& currentMousePos);
	void init(SGA::TBSGameState& state) override;
	void update(SGA::TBSGameState& state) override;
	void draw(SGA::TBSGameState& state, sf::RenderWindow& window)const override;
private:
	std::vector<SGA::Action<SGA::Vector2i>>& actionSpace;
	std::vector<sf::CircleShape> circleShapes;

	SGA::Vector2f& currentMousePos;
	
};
