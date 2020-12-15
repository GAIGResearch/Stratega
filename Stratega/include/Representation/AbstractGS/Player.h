#pragma once
#include <functional>

namespace SGA
{
	struct GameState;
	struct Entity;
	
	class Player
	{
	public:
		Player(int id, GameState& state);
		int id;
		double score;
		bool canPlay;

		//State reference
		std::reference_wrapper<GameState> state;

		std::vector<Entity*> getEntities() const;
	};
}
