#pragma once

namespace SGA
{
	class EntityForwardModel;
	struct GameState;
	
	class InterruptableAI
	{
	public:
		InterruptableAI(int playerID);
		
		virtual void init(const GameState& initialState, int budget) = 0;
		virtual void computeAction(GameState state, EntityForwardModel fm, int budget) = 0;

		[[nodiscard]] int getPlayerID() const;
		
	private:
		int playerID;
	};
}