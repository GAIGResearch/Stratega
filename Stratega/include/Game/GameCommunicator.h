#pragma once

namespace SGA
{
	class GameCommunicator
	{
	public:
		virtual ~GameCommunicator() = default;
		GameCommunicator(int playerID):
			playerID(playerID)
		{
			
		}

		int getPlayerID() const { return playerID; }
		virtual void init() {};
		virtual void close() {};
		virtual void onGameStateAdvanced() {};
	
	private:
		int playerID;
	};	
}
