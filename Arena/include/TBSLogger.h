#pragma once
#include <Stratega/Game/GameCommunicator.h>
#include <Stratega/Game/TBSGame.h>

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