#include <Representation/TBSGameState.h>

namespace SGA
{
	TBSGameState::TBSGameState()
		: TBSGameState(Board(0, 0), {}, {})
	{
	}
	
	TBSGameState::TBSGameState(Board&& board, const std::unordered_map<int, UnitType>& unitTypes, const std::unordered_map<int, TileType>& tileTypes):
		fogOfWarTile(-1, 0, 0),
		isGameOver(false),
		currentGameTurn(0),
		currentPlayer(0),
		roundLimit(100),
		board(std::move(board)),
		units(),
		players(),
		lastUsedPlayerID(-1),
		lastUsedUnitID(-1),
		winnerPlayerID(-1),
		unitTypes(std::make_shared<std::unordered_map<int, UnitType>>(unitTypes)),
		tileTypes(std::make_shared<std::unordered_map<int, TileType>>(tileTypes))
		
	{
	}

	TBSGameState::TBSGameState(const TBSGameState& copy) noexcept:
		fogOfWarTile(copy.fogOfWarTile),
		isGameOver(copy.isGameOver),
		currentGameTurn(copy.currentGameTurn),
		currentPlayer(copy.currentPlayer),
		roundLimit(copy.roundLimit),
		board(copy.board),
		units(copy.units),
		players(copy.players),
		lastUsedPlayerID(copy.lastUsedPlayerID),
		lastUsedUnitID(copy.lastUsedUnitID),
		winnerPlayerID(copy.winnerPlayerID),
		unitTypes(copy.unitTypes),
		tileTypes(copy.tileTypes)
	{
		setOwner();
	}

	TBSGameState::TBSGameState(TBSGameState&& other) noexcept
		: TBSGameState()
	{
		swap(*this, other);
	}

	TBSGameState& TBSGameState::operator=(TBSGameState other) noexcept
	{
		swap(*this, other);
		return *this;
	}

	void TBSGameState::setOwner() noexcept
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

	TBSPlayer* TBSGameState::getPlayer(int playerID)
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

	int TBSGameState::addUnit(int playerID, int typeID, const Vector2i& position)
	{
		auto type = unitTypes->at(typeID);
		
		lastUsedUnitID++;
		auto& unit = units.emplace_back(*this, lastUsedUnitID, playerID);
		unit.setTypeID(type.id);
		unit.setAttackDamage(type.attackDamage);
		unit.setHealAmount(type.healAmount);
		unit.setHealth(type.maxHealth);
		unit.setLineOfSightRange(type.lineOfSightRange);
		unit.setPosition(position);
		unit.setRange(type.movementRange);
		return lastUsedUnitID;
	}

	int TBSGameState::addPlayer()
	{
		lastUsedPlayerID++;
		players.emplace_back(*this, lastUsedPlayerID);
		return lastUsedPlayerID;
	}

	void TBSGameState::removeUnit(int unitID)
	{
		auto it = std::find_if(units.begin(), units.end(),
			[=](const TBSUnit& p) {return p.getUnitID() == unitID; });
		
		if (it != units.end())
			units.erase(it);
	}

	TBSUnit* TBSGameState::getUnit(int unitID)
	{
		for (TBSUnit& unit : units)
		{
			if (unit.getUnitID() == unitID)
			{
				return &unit;
			}
		}
		return nullptr;

	}

	TBSUnit* TBSGameState::getUnit(Vector2i pos)
	{
		for (TBSUnit& unit : units)
		{
			if (unit.getPosition() == pos)
			{
				return &unit;
			}
		}
		return nullptr;

	}

	void TBSGameState::printGameStateStatus()
	{
		auto string = "Current player turn: " +
			std::to_string(currentPlayer) +
			"/" +
			std::to_string(players.size() - 1) +
			", Current game turn " +
			std::to_string(currentGameTurn);

		std::cout << string << std::endl;


		std::cout << std::endl << "Unit list: " << std::endl;
		for (TBSUnit unit : units)
		{
			std::cout << "Unit: " << unit.getUnitID() << std::endl;
		}

		for (int i = 0; i < board.getHeight(); ++i)
		{
			for (int j = 0; j < board.getWidth(); ++j)
			{


				if (!getUnit(Vector2i(i, j)))
					std::cout << 'X';
				else
					std::cout << getUnit(Vector2i(i, j))->getUnitID();
			}
			std::cout << std::endl;
		}

	}

	void TBSGameState::applyFogOfWar(int playerID)
	{
		auto* targetPlayer = getPlayer(playerID);
		auto playerUnits = targetPlayer->getUnits();

		// Helper method
		auto isVisible = [&](const Vector2i& pos)
		{
			for (auto* pu : playerUnits)
			{
				if (pu->getPosition().distance(pos) <= pu->getLineOfSightRange())
					return true;
			}
			return false;
		};
		
		// Remove units that are not visible
		auto it = units.begin();
		while(it != units.end())
		{
			if(it->getPlayerID() != targetPlayer->playerID && !isVisible(it->getPosition()))
			{
				it = units.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Hide tiles that are not visible
		for(int y = 0; y < board.getHeight(); y++)
		{
			for (int x = 0; x < board.getWidth(); x++)
			{
				if(!isVisible(Vector2i(x, y)))
				{
					auto& tile = board.getTile(x, y);
					tile = fogOfWarTile;
					tile.position = Vector2i(x, y);
				}
			}
		}
	}

}
