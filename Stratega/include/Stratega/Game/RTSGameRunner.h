#pragma once
#include <Stratega/Game/GameRunner.h>

namespace SGA
{
	class RTSGameRunner final : public GameRunner
	{
	public:
		explicit RTSGameRunner(const GameConfig& config);

	protected:
		void runInternal(std::vector<std::unique_ptr<Agent>>& agents) override;
		void playInternal(std::vector<std::unique_ptr<Agent>>& agents, int humanIndex) override;
	};
}