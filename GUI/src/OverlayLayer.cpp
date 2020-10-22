#include <OverlayLayer.h>

TBSOverlayLayer::TBSOverlayLayer(AssetCache& assetCache, SGA::ActionSpace<SGA::Vector2i>& actionSpace, SGA::Vector2f& currentMousePos) :
	RenderLayer(assetCache), actionSpace(actionSpace), currentMousePos(currentMousePos)
{

}

void TBSOverlayLayer::init(SGA::TBSGameState& state)
{
	

}

void TBSOverlayLayer::update(SGA::TBSGameState& state)
{
	//Clean previous sprites
	circleShapes.clear();
	sprites.clear();

	//Add actions if we have actions to draw
	if(actionSpace.count()>0)
	{
		for(const auto& action : actionSpace)
		{
			sf::CircleShape shape(15);
			sf::Vector2f temp = toISO(action.getTargetPosition().x, action.getTargetPosition().y);
		
			shape.setPosition(temp+sf::Vector2f(TILE_OFFSET_ORIGIN_X,TILE_OFFSET_ORIGIN_Y));
			switch (action.getType())
			{
				case SGA::ActionType::Attack: shape.setFillColor(sf::Color::Red); break;
				case SGA::ActionType::Move:  break;
				case SGA::ActionType::Heal: shape.setFillColor(sf::Color::Green);  break;
				case SGA::ActionType::Push: shape.setFillColor(sf::Color::Black);  break;
				case SGA::ActionType::EndTurn:  shape.setFillColor(sf::Color::Magenta); break;
				default: throw std::runtime_error("Tried adding an action with an not supported action-type");
			}
			circleShapes.emplace_back(shape);
		}		
	}

	//Add selected tile
	sf::Vector2i mouseGridPos = toGrid(sf::Vector2f(currentMousePos.x, currentMousePos.y));
	
	if(state.isInBounds(SGA::Vector2i(mouseGridPos.x,mouseGridPos.y)))
	{
		sf::Texture& texture = assetCache.getTexture("selected");
		sf::Vector2f origin(TILE_ORIGIN_X, TILE_ORIGIN_Y);
		sf::Sprite selectedTile(texture);
	
		selectedTile.setPosition(toISO(mouseGridPos.x, mouseGridPos.y));
		selectedTile.setOrigin(origin);
		sprites.emplace_back(selectedTile);
	}
	
}

void TBSOverlayLayer::draw(SGA::TBSGameState& state, sf::RenderWindow& window) const
{
	for (const auto& circleShape : circleShapes)
	{
		window.draw(circleShape);
	}

	for (const auto& sprite : sprites)
	{
		window.draw(sprite);
	}
}

RTSOverlayLayer::RTSOverlayLayer(AssetCache& assetCache,  SGA::Vector2f& currentMousePos) :
	RenderLayer(assetCache),  currentMousePos(currentMousePos)
{

}

void RTSOverlayLayer::init(SGA::RTSGameState& state)
{


}

void RTSOverlayLayer::update(SGA::RTSGameState& state)
{
}

void RTSOverlayLayer::draw(SGA::RTSGameState& state, sf::RenderWindow& window) const
{
	const dtNavMesh* mesh=state.navigation->m_navMesh;

	if (mesh)
	{
		//Draw navmesh
		for (int i = 0; i < state.navigation->m_navMesh->getMaxTiles(); ++i)
		{
			const dtMeshTile* tile = mesh->getTile(i);

			if (!tile->header) continue;
			{
				for (int i = 0; i < tile->header->polyCount; ++i)
				{					
					const dtPoly* p = &tile->polys[i];
					if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
						continue;

					const dtPolyDetail* pd = &tile->detailMeshes[i];

					//Draw polygon
					for (int j = 0; j < pd->triCount; ++j)
					{
						const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];

						sf::ConvexShape polygon;
						
						//Adjust to offset from isometric
						polygon.move(TILE_WIDTH_HALF,0);

						polygon.setOutlineColor(sf::Color::Black);
						polygon.setFillColor(sf::Color(20, 140, 240, 64));
						polygon.setOutlineThickness(1);
						polygon.setPointCount(3);
						for (int k = 0; k < 3; ++k)
						{							
							if (t[k] < p->vertCount)
							{								
								float* pos = &tile->verts[p->verts[t[k]] * 3];								
								polygon.setPoint(k, toISO(pos[0], pos[2]));							
							}								
						}
						window.draw(polygon);
					}
				}
			}
		}
	}

	std::vector<SGA::RTSUnit> units = state.getUnits();

	//Draw paths of units
	for (auto& unit : units)
	{
		if (unit.executingAction) {
			
			//Check if has a valid path
			if (unit.path.m_nstraightPath > 0)
			{
				//Draw path lines
				for (int i = 0; i < unit.path.m_nstraightPath - 1; ++i)
				{
					auto v1 = toISO(unit.path.m_straightPath[i * 3], unit.path.m_straightPath[i * 3 + 2]);
					auto v2 = toISO(unit.path.m_straightPath[(i + 1) * 3], unit.path.m_straightPath[(i + 1) * 3 + 2]);

					//Adjust to sprites offset
					v1.x += TILE_WIDTH_HALF;
					v2.x += TILE_WIDTH_HALF;

					sf::Vertex line[] =
					{
						sf::Vertex(v1),
						sf::Vertex(v2)
					};

					window.draw(line, 2, sf::Lines);
				}

				//Draw path points
				for (int i = 0; i < unit.path.m_nstraightPath; ++i)
				{
					sf::CircleShape pathCircle;
					float v1 = unit.path.m_straightPath[i * 3];
					float v2 = unit.path.m_straightPath[i * 3 + 2];

					auto pos = toISO(v1, v2);
					pathCircle.setPosition(pos);
					pathCircle.setOrigin(10, 10);
					pathCircle.setFillColor(sf::Color::Black);
					pathCircle.setRadius(20);
					pathCircle.move(TILE_WIDTH_HALF, 0);
					window.draw(pathCircle);
				}

				//Paint Start and End circles
				sf::CircleShape temp;
				temp.setOrigin(10, 10);
				temp.setPosition(toISO(unit.position.x, unit.position.y));
				temp.move(TILE_WIDTH_HALF, 0);
				temp.setFillColor(sf::Color::Green);
				temp.setRadius(20);
				window.draw(temp);

				temp.setPosition(toISO(unit.executingAction->getTargetPosition().x, unit.executingAction->getTargetPosition().y));
				temp.move(TILE_WIDTH_HALF, 0);
				temp.setFillColor(sf::Color::Red);
				temp.setRadius(20);
				window.draw(temp);
			}
		}
	}
}