#include <UnitLayer.h>

TBSUnitLayer::TBSUnitLayer(AssetCache& assetCache)	:
	RenderLayer(assetCache)
{

}

void TBSUnitLayer::init( SGA::TBSGameState& state)
{
	const std::vector<SGA::TBSUnit>& units = state.getUnits();

	for(auto& unit: units)
	{
		sf::Texture& texture = assetCache.getTexture("unit_" + std::to_string(unit.getUnitTypeID()));
		sf::Vector2f origin(UNIT_OFFSET_ORIGIN_X, UNIT_OFFSET_ORIGIN_Y);
		sf::Sprite newUnit(texture);

		newUnit.setPosition(toISO(unit.getPosition().x, unit.getPosition().y));
		newUnit.setOrigin(origin);
		sprites.emplace_back(newUnit);
	}
	
}

void TBSUnitLayer::update(SGA::TBSGameState& state)
{
	sprites.clear();
	unitsInfo.clear();
	const std::vector<SGA::TBSUnit>& units = state.getUnits();
	for (auto& unit : units)
	{
		//Add units
		sf::Texture& texture = assetCache.getTexture("unit_" + std::to_string(unit.getUnitTypeID()));
		sf::Vector2f origin(0, texture.getSize().y/1.4);
		sf::Sprite newUnit(texture);

		sf::Vector2f pos = toISO(unit.getPosition().x, unit.getPosition().y);
		newUnit.setPosition(pos.x+TILE_WIDTH_HALF/2,pos.y+TILE_HEIGHT_HALF/2);
		
		newUnit.setOrigin(origin);
		sprites.emplace_back(newUnit);

		//Add units text info
		sf::Text unitInfo;
		unitInfo.setFont(assetCache.getFont("font"));
		
		unitInfo.setString("PlayerID: " + std::to_string(unit.getPlayerID())+"ID: " + std::to_string(unit.getUnitID()) + " " + std::to_string(unit.getHealthh()));
		unitInfo.setPosition(toISO(unit.getPosition().x, unit.getPosition().y));
		unitsInfo.emplace_back(unitInfo);
		
	}
}

void TBSUnitLayer::draw(SGA::TBSGameState& state, sf::RenderWindow& window) const
{
	for(auto& sprite : sprites)
	{
		window.draw(sprite);
	}

	for (auto& text : unitsInfo)
	{
		window.draw(text);
	}
}