#include <Representation/Player.h>
#include <Representation/GameState.h>
#include <Representation/Entity.h>
namespace SGA
{
	Player::Player(int id, GameState& state) :
		id(id),
		score(0),
		canPlay(true),
		state(state)
	{

	}

	std::vector<Entity*> Player::getEntities() const
	{
		std::vector<Entity*> entitiesOfPlayer;
		for (auto& entity : state.get().entities)
		{
			if (entity.owner == id)
			{
				entitiesOfPlayer.emplace_back(&entity);
			}
		}
		return entitiesOfPlayer;
	}

}
