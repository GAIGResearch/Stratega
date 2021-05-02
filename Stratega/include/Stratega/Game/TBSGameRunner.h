#pragma once
#include <Stratega/Game/GamerRunner.h>

namespace SGA
{
	class TBSGameRunner final : public GameRunner
	{
	public:
		explicit TBSGameRunner(const GameConfig& config);

	protected:
		void runInternal(std::vector<std::unique_ptr<Agent>>& agents) override;
		void playInternal(std::vector<std::unique_ptr<Agent>>& agents, int humanIndex) override;
	};
}
