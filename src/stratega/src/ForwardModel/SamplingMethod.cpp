#include <queue>
#include <Stratega/ForwardModel/SamplingMethod.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Representation/Vector2.h>
#include <cmath>
#pragma warning(disable: 5045)
std::vector<SGA::Vector2i> SGA::SamplingMethod::getPositions(const GameState& gameState) const
{
	std::vector<SGA::Vector2i> targets;

	auto isValidPos = [&](int x, int y)
	{
		return gameState.getTileAt({ x, y }).getTileTypeID() != -1;
	}; 

	for (int x = 0; x < gameState.getBoardWidth(); x++)
	{
		for (int y = 0; y < gameState.getBoardHeight(); y++)
		{
			if (isValidPos(x, y))
				targets.emplace_back(x, y);
		}
	}

	return targets;
}

std::vector<int> SGA::SamplingMethod::getEntities(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs) const
{
	std::vector<int> targets;
	for (const auto& entity : gameState.getEntities())
	{
		if (entityTypeIDs.find(entity.getEntityTypeID()) != entityTypeIDs.end())
		{
			targets.emplace_back(entity.getID());
		}
	}
	return targets;
}

bool SGA::SamplingMethod::validatePosition(const GameState& /*gameState*/, const Vector2f& /*sourcePosition*/, const Vector2f& /*targetPosition*/) const
{
	return true;
}

bool SGA::SamplingMethod::validatePosition(const GameState& gameState, const Vector2f& targetPosition) const
{
	return gameState.getTileAt({ static_cast<int>(targetPosition.x), static_cast<int>(targetPosition.y)}).getTileTypeID() != -1;
}

std::vector<SGA::Vector2i> SGA::Neighbours::getPositions(const GameState& gameState, const Vector2f& position) const
{
	std::vector<SGA::Vector2i> targets;

	//Check all positions
	if (shapeType == ShapeType::AllPositions)
	{
		auto isValidPos = [&](int x, int y)
		{
			return gameState.getTileAt({ x, y }).getTileTypeID() != -1;
		};

		for (int x = 0; x < gameState.getBoardWidth(); x++)
		{
			for (int y = 0; y < gameState.getBoardHeight(); y++)
			{
				if (isValidPos(x, y))
					targets.emplace_back(x, y);
			}
		}
	}
	else
	{
		auto isValidPos = [&](int x, int y)
		{
			if (gameState.getTileAt({ x, y }).getTileTypeID() == -1)
				return false;

			switch (shapeType)
			{
			case ShapeType::Square: return true;
			case ShapeType::Circle: return Vector2f(x, y).distance(position) <= shapeSize;
			default: return false;
			}
		};
		
		// Iterate over an rectangle as large as 'shapeSize' and take every valid position
		auto startCheckPositionX = std::max<int>(0, static_cast<int>(position.x) - shapeSize);
		auto endCheckPositionX = std::min<int>(gameState.getBoardWidth() - 1, static_cast<int>(position.x) + shapeSize);
		auto startCheckPositionY = std::max<int>(0, static_cast<int>(position.y - shapeSize));
		auto endCheckPositionY = std::min<int>(gameState.getBoardHeight() - 1, static_cast<int>(position.y) + shapeSize);

		for (auto x = startCheckPositionX; x <= endCheckPositionX; x++)
		{
			for (auto y = startCheckPositionY; y <= endCheckPositionY; y++)
			{
				if (isValidPos(x, y))
					targets.emplace_back(x,y);
			}
		}
	}

	return targets;
}

std::vector<int> SGA::Neighbours::getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& /*entityTypeIDs*/) const
{
	std::vector<int> entitiesIDs;
	//Call base method
	for (auto& entity : gameState.getEntities())
	{
		if (shapeType==ShapeType::AllPositions||entity.getPosition().distance(position) <= shapeSize)
			entitiesIDs.emplace_back(entity.getID());
	}
	return entitiesIDs;
}

bool SGA::Neighbours::validatePosition(const GameState& /*gameState*/, const Vector2f& sourcePosition, const Vector2f& targetPosition) const
{
	switch (shapeType)
	{
	case ShapeType::Square:			
	{
		return (sourcePosition.x >= targetPosition.x - shapeSize && sourcePosition.x <= targetPosition.x + shapeSize &&
			sourcePosition.y>=targetPosition.y - shapeSize && sourcePosition.y <= targetPosition.y + shapeSize);
	}				
	case ShapeType::Circle:
	{
		return sourcePosition.distance(targetPosition) <= shapeSize;
	}				
	default: return true;
	}
}

std::vector<SGA::Vector2i> SGA::Dijkstra::getPositions(const GameState& gameState, const Vector2f& position) const
{
	std::vector<SGA::Vector2i> positions;
	
	Node root(static_cast<int>(position.x), static_cast<int>(position.y));
	root.visited = true;
	root.totalCost = 0;

	std::vector<Node> nodesCache;
	std::vector<Node> possibleDestinations;

	Node currentNode;

	std::priority_queue<Node> openNodes;
	std::vector<Node> visitedNodes;

	visitedNodes.emplace_back(root);
	openNodes.emplace(root);
	nodesCache.emplace_back(root);

	while(!openNodes.empty())
	{
		currentNode = openNodes.top();
		openNodes.pop();

		//Check if the node is in possible destinations
		auto it = std::find(possibleDestinations.begin(), possibleDestinations.end(), currentNode);
		if(it==possibleDestinations.end())
		{
			possibleDestinations.emplace_back(currentNode);
			positions.emplace_back(currentNode.posX, currentNode.posY);
		}
				
		//Get neighbours
		std::vector<Node> neighbours;
		{
			auto startCheckPositionX = std::max<int>(0, currentNode.posX - 1);
			auto endCheckPositionX = std::min<int>(gameState.getBoardWidth() - 1, currentNode.posX + 1);
			auto startCheckPositionY = std::max<int>(0, currentNode.posY - 1);
			auto endCheckPositionY = std::min<int>(gameState.getBoardHeight() - 1, currentNode.posY + 1);

			auto isValidPos = [&](int x, int y, float totalCost)
			{
				if (gameState.getTileAt({ x, y }).getTileTypeID() == -1 || !gameState.getTileAt({ x, y }).isWalkable()
					|| std::floor(totalCost + 1) > static_cast<float>(searchSize))
					return false;		
				else
					return true;
			};

			//Get Neighbours
			for (auto x = startCheckPositionX; x <= endCheckPositionX; x++)
			{
				for (auto y = startCheckPositionY; y <= endCheckPositionY; y++)
				{
					//Allow diagonals
					if (!allowDiagonals)
						if ((x == startCheckPositionX && y == startCheckPositionY) ||
							(x == endCheckPositionX && y == endCheckPositionY) ||
							(x == startCheckPositionX && y == endCheckPositionY) ||
							(x == endCheckPositionX && y == startCheckPositionY))
							continue;

					//Valid neighbors
					if (isValidPos(x, y, currentNode.totalCost))
					{
						Node newNeighbour{ x,y };
						newNeighbour.totalCost = currentNode.totalCost + 1;
						neighbours.emplace_back(newNeighbour);
					}
				}
			}
		}

		//Check neighbours
		for (auto& currentNeighbour : neighbours)
		{
			float nbCost = currentNeighbour.totalCost;

			bool found = false;
			Node neighbour;
			if(!nodesCache.empty())
			{
				for (auto& nodeInCache : nodesCache)
				{
					if(nodeInCache==currentNeighbour)
					{
						found = true;
						neighbour = nodeInCache;
						break;
					}
				}
			}

			if (!found)
				neighbour = currentNeighbour;

			auto it2 = std::find(visitedNodes.begin(), visitedNodes.end(), neighbour);
			if (it2 == visitedNodes.end())
			{
				neighbour.visited = true;
				visitedNodes.emplace_back(neighbour);
				neighbour.totalCost = nbCost+currentNode.totalCost;
				openNodes.push(neighbour);
				nodesCache.emplace_back(neighbour);
			}
			else if(nbCost +currentNode.totalCost < neighbour.totalCost)
			{
				neighbour.totalCost= nbCost + currentNode.totalCost;
			}
		}
	}
	
	return positions;
}

std::vector<int> SGA::Dijkstra::getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& /*entityTypeIDs*/) const
{
	//Call base method
	auto possiblePositions = getPositions(gameState, position);
	
	std::vector<int> entities;
	
	for (auto& pos : possiblePositions)
	{
		auto* entity = gameState.getEntityAt(Vector2f(pos.x, pos.y));
		if(entity)
		{
			entities.emplace_back(entity->getID());
		}
	}
	
	return entities;
}

bool SGA::Dijkstra::validatePosition(const GameState& gameState, const Vector2f& sourcePosition, const Vector2f& targetPosition) const
{
	//Take Dijkstra possible positions
	auto positions = getPositions(gameState, sourcePosition);

	for(auto& position : positions)
	{
		if (targetPosition.distance(Vector2f(static_cast<float>(position.x), static_cast<float>(position.y))) <= static_cast<double>(0.5f))
			return true;
	}

	return false;
}