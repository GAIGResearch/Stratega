#pragma once
#include <Game/GameCommunicator.h>
#include <Game/Game.h>

class TBSLogger : public SGA::GameCommunicator
{
public:
	TBSLogger(SGA::TBSGame& game)
		: GameCommunicator(-1), game(&game)
	{
	}

	void close() override;
	void onGameStateAdvanced() override;
private:
	SGA::TBSGame* game;
};