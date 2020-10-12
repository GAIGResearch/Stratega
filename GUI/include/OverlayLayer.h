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
	TBSOverlayLayer(AssetCache& assetCache, SGA::ActionSpace<SGA::Vector2i>& actionSpace, SGA::Vector2f& currentMousePos);
	void init(SGA::TBSGameState& state) override;
	void update(SGA::TBSGameState& state) override;
	void draw(SGA::TBSGameState& state, sf::RenderWindow& window)const override;
private:
	SGA::ActionSpace<SGA::Vector2i>& actionSpace;
	std::vector<sf::CircleShape> circleShapes;

	SGA::Vector2f& currentMousePos;
	
};

class RTSOverlayLayer : public RenderLayer<SGA::RTSGameState>
{
public:
	RTSOverlayLayer(AssetCache& assetCache, SGA::Vector2f& currentMousePos);
	void init(SGA::RTSGameState& state) override;
	void update(SGA::RTSGameState& state) override;
	void draw(SGA::RTSGameState& state, sf::RenderWindow& window)const override;
private:
	
	std::vector<sf::Sprite> boxColliderShapes;
	std::vector<sf::Sprite> circleColliderShapes;

	SGA::Vector2f& currentMousePos;

};
