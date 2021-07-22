#pragma once
#include <Stratega/Game/GameRunner.h>

namespace SGA
{
	class AgentThread;
	class RTSGameRunner final : public GameRunner
	{
	public:
		explicit RTSGameRunner(const GameConfig& config);

	protected:
		void runInternal(std::vector<Agent*>& agents, GameObserver& observer) override;
		void playInternal(std::vector<Agent*>& agents, int humanIndex) override;
		bool checkComputationTime(std::chrono::milliseconds computationTime, int currentPlayerID);
	};
}