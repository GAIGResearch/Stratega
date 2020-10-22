#include <RTSUnitLayer.h>


RTSUnitLayer::RTSUnitLayer(AssetCache& assetCache, RTSGameStateRender& renderer) :
	RenderLayer(assetCache), renderer(&renderer)
{

}

void RTSUnitLayer::init(SGA::RTSGameState& state)
{

}

void RTSUnitLayer::update(SGA::RTSGameState& state)
{
	sprites.clear();
	unitsInfo.clear();
	healthBars.clear();

	const std::vector<SGA::RTSUnit>& units = state.units;
	for (auto& unit : units)
	{

		//Add units
		sf::Texture& texture = assetCache.getTexture("unit_" + std::to_string(unit.unitTypeID));
		sf::Vector2f origin(texture.getSize().x / 2, texture.getSize().y / 1.4);
		//sf::Vector2f origin(0, texture.getSize().y / 1.4);
		sf::Sprite newUnit(texture);

		sf::Vector2f pos = toISO(unit.position.x, unit.position.y);
		//newUnit.setPosition(pos.x + TILE_WIDTH_HALF / 2, pos.y + TILE_HEIGHT_HALF / 2);
		newUnit.setPosition(pos.x + TILE_WIDTH_HALF, pos.y);

		newUnit.setOrigin(origin);
		sprites.emplace_back(newUnit);

		//Add units text info
		sf::Text unitInfo;
		unitInfo.setFont(assetCache.getFont("font"));

		unitInfo.setString("PlayerID: " + std::to_string(unit.playerID) + "ID: " + std::to_string(unit.unitID) + " " + std::to_string(unit.health));
		unitInfo.setPosition(toISO(unit.position.x, unit.position.y));
		unitsInfo.emplace_back(unitInfo);

		//Add temporal Health bar
		sf::RectangleShape background;
		sf::Vector2f backgroundSize(140, 35);
		sf::Vector2f fillSize(130, 25);

		int yOffset = -220;

		background.setPosition(pos.x + TILE_WIDTH_HALF, pos.y + yOffset);
		background.setFillColor(sf::Color::Black);
		background.setSize(backgroundSize);
		background.setOrigin(backgroundSize.x / 2, backgroundSize.y / 2);
		healthBars.emplace_back(background);

		sf::RectangleShape fill;
		fill.setPosition(pos.x + TILE_WIDTH_HALF, pos.y + yOffset);
		fill.setFillColor(sf::Color::Red);
		fill.setSize(fillSize);
		fill.setOrigin(fillSize.x / 2, fillSize.y / 2);
		healthBars.emplace_back(fill);

	}
}

void RTSUnitLayer::draw(SGA::RTSGameState& state, sf::RenderWindow& window) const
{
	for (auto& sprite : sprites)
	{
		sf::CircleShape temp(10);
		temp.setPosition(sprite.getPosition());
		temp.setFillColor(sf::Color::Cyan);
		window.draw(sprite);
		window.draw(temp);
	}

	for (auto& text : unitsInfo)
	{
		window.draw(text);
	}

	for (auto& bar : healthBars)
	{
		window.draw(bar);
	}

	//Check if units are selected
	const std::vector<SGA::RTSUnit>& units = state.units;
	for (const auto& unit : units)
	{
		if (renderer->isSelected(unit.unitID))
		{
			auto& selectionTexture = assetCache.getTexture("circleCollider");
			sf::Sprite selectionSprite(selectionTexture);
			selectionSprite.setOrigin(selectionTexture.getSize().x / 2, selectionTexture.getSize().y/2);
			selectionSprite.setPosition(toISO(unit.position.x, unit.position.y) + sf::Vector2f{ TILE_WIDTH_HALF, 0 });
			selectionSprite.setColor(sf::Color::Blue);
			window.draw(selectionSprite);
		}
	}
}