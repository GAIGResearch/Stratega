#include <Stratega/ForwardModel/SamplingMethod.h>
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Representation/Vector2.h>

std::vector<SGA::Vector2i> SGA::SamplingMethod::getPositions(const GameState& gameState) const
{
	std::vector<SGA::Vector2i> targets;

	auto isValidPos = [&](int x, int y)
	{
		return gameState.board.get(x, y).tileTypeID != -1;
	};

	for (int x = 0; x < static_cast<int>(gameState.board.getWidth()); x++)
	{
		for (int y = 0; y < static_cast<int>(gameState.board.getHeight()); y++)
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
	for (const auto& entity : gameState.entities)
	{
		if (entityTypeIDs.find(entity.typeID) != entityTypeIDs.end())
		{
			targets.emplace_back(entity.id);
		}
	}
	return targets;
}


std::vector<SGA::Vector2i> SGA::Neighbours::getPositions(const GameState& gameState, const Vector2f& position) const
{
	std::vector<SGA::Vector2i> targets;

	//Check all positions
	if (shapeType == ShapeType::AllPositions)
	{
		auto isValidPos = [&](int x, int y)
		{
			return gameState.board.get(x, y).tileTypeID != -1;
		};

		for (int x = 0; x < static_cast<int>(gameState.board.getWidth()); x++)
		{
			for (int y = 0; y < static_cast<int>(gameState.board.getHeight()); y++)
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
			if (gameState.board.get(x, y).tileTypeID == -1)
				return false;

			switch (shapeType)
			{
			case ShapeType::Square: return true;
			case ShapeType::Circle: return Vector2f(x, y).distance(position) <= shapeSize;
			default: return false;
			}
		};
		
		// Iterate over an rectangle as large as 'shapeSize' and take every valid position
		auto startCheckPositionX = std::max<int>(0, static_cast<int>(position.x - shapeSize));
		auto endCheckPositionX = std::min<int>(static_cast<int>(gameState.board.getWidth() - 1), static_cast<int>(position.x + shapeSize));
		auto startCheckPositionY = std::max<int>(0, static_cast<int>(position.y - shapeSize));
		auto endCheckPositionY = std::min<int>(static_cast<int>(gameState.board.getHeight() - 1), static_cast<int>(position.y + shapeSize));

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

std::vector<int> SGA::Neighbours::getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs) const
{
	//Call base method
	return SamplingMethod::getEntities(gameState, entityTypeIDs);
}

