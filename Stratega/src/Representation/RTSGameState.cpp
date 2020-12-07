#include <Representation/RTSGameState.h>

namespace SGA
{
	RTSGameState::RTSGameState()
		: fogOfWarTile(-1, 0, 0),
		isGameOver(false),
		board(0, 0),
		lastUsedPlayerID(0),
		lastUsedUnitID(0),
		winnerPlayerID(0),
		tileScale(1),
		navigation(std::make_shared<Navigation>())
	{
		
	}	

	RTSGameState::RTSGameState(const RTSGameState& copy) noexcept :
		fogOfWarTile(copy.fogOfWarTile),
		unitTypes(copy.unitTypes),
		tileTypes(copy.tileTypes),
		isGameOver(copy.isGameOver),
		board(copy.board),
		units(copy.units),
		players(copy.players),
		lastUsedPlayerID(copy.lastUsedPlayerID),
		lastUsedUnitID(copy.lastUsedUnitID),
		winnerPlayerID(copy.winnerPlayerID),
		navigation(copy.navigation)
	{
 		setOwner();
	}

	RTSGameState::RTSGameState(RTSGameState&& other) noexcept
		: RTSGameState()
	{
		swap(*this, other);
	}

	RTSGameState& RTSGameState::operator=(RTSGameState other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	void RTSGameState::setOwner() noexcept
	{
		for (auto& player : players)
		{
			player.state = *this;
		}

		for (auto& unit : units)
		{
			unit.state = *this;
		}
	}


	RTSUnit* RTSGameState::getUnit(int unitID)
	{
		for (auto& unit : units)
		{
			if (unit.unitID == unitID)
			{
				return &unit;
			}
		}
		return nullptr;
	}

	RTSUnit* RTSGameState::getUnit(Vector2f pos, float maxDistance)
	{
		for (auto& unit : units)
		{
			if (unit.position.distance(pos) <= maxDistance)
			{
				return &unit;
			}
		}
		return nullptr;
	}

	RTSPlayer* RTSGameState::getPlayer(int playerID)
	{
		for (auto& player : players)
		{
			if (player.playerID == playerID)
			{
				return &player;
			}
		}

		return nullptr;
	}

	int RTSGameState::addPlayer()
	{
		lastUsedPlayerID++;
		players.emplace_back(*this, lastUsedPlayerID);
		return lastUsedPlayerID;
	}
	
	int RTSGameState::addUnit(int playerID, int typeID, const Vector2i& position)
	{
		auto type = unitTypes->at(typeID);

		lastUsedUnitID++;
		auto& unit = units.emplace_back(*this, lastUsedUnitID, playerID);
		unit.unitTypeID = type.id;
		unit.attackDamage = type.attackDamage;
		unit.healAmount = type.healAmount;
		unit.health = type.maxHealth;
		unit.maxHealth = type.maxHealth;
		unit.actionRange = type.actionRange;
		unit.position = Vector2f(position.x,position.y);
		unit.collisionRadius = type.collisionRadius;
		unit.lineOfSightRange = type.lineOfSightRange;
		return lastUsedUnitID;
	}

	void RTSGameState::removeUnit(int id)
	{
		auto it = std::find_if(units.begin(), units.end(),
			[&](const RTSUnit& p) {return p.unitID == id; });

		if (it != units.end())
			units.erase(it);
	}

	void RTSGameState::applyFogOfWar(int playerID)
	{
		auto* targetPlayer = getPlayer(playerID);
		// Helper method
		auto isVisible = [&](const Vector2f& pos)
		{
			for (const auto& unit : units)
			{
				if (unit.playerID == playerID && unit.position.distance(pos) <= unit.lineOfSightRange)
				{
					return true;
				}
			}
			return false;
		};

		// Remove units that are not visible
		auto it = units.begin();
		while (it != units.end())
		{
			if (it->playerID != targetPlayer->playerID && !isVisible(it->position))
			{
				it = units.erase(it);
			}
			else
			{
				//Opponents should not see other units intentions
				it->intendedAction.type = RTSActionType::None;
				++it;
			}
		}

		// Hide tiles that are not visible
		for (int y = 0; y < board.getHeight(); y++)
		{
			for (int x = 0; x < board.getWidth(); x++)
			{
				if (!isVisible(Vector2i(x, y)))
				{
					auto& tile = board.getTile(x, y);
					tile = fogOfWarTile;
					tile.position = Vector2i(x, y);
				}
			}
		}

		fogOfWarId = playerID;
	}
}