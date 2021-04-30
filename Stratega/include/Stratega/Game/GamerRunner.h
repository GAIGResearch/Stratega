#pragma once

namespace SGA
{
	class GameRunner
	{
	public:
		virtual ~GameRunner() = default;
		
		virtual void reset() = 0;
		virtual void step(const ActionAssignment& actions) = 0;
		virtual void render() = 0;
		virtual void run(std::vector<std::unique_ptr<Agent>>& agents) = 0;
		virtual void play(std::vector<std::unique_ptr<Agent>>& agents) = 0;

		[[nodiscard]] const GameState& getGameState() const;
	};
}
