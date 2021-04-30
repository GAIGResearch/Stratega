#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/EntityForwardModel.h>
// ToDo we shouldn't need to include configuration
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Agent/Agent.h>
#include <Stratega/GUI/TBSGameRenderer.h>
#include <Stratega/Game/GamerRunner.h>

namespace SGA
{
	class TBSGameRunner final : public GameRunner
	{
	public:
		TBSGameRunner(std::unique_ptr<GameConfig> config);

		void reset() override;
		void step(const ActionAssignment& actions) override;
		void render() override;
		void run(std::vector<std::unique_ptr<Agent>>& agents) override;
		void play(std::vector<std::unique_ptr<Agent>>& agents) override;

		[[nodiscard]] const GameState& getGameState() const;

	private:
		void ensureRendererInitialized();
		
		
		std::unique_ptr<GameConfig> config;
		
		std::unique_ptr<GameState> currentState;
		std::unique_ptr<EntityForwardModel> forwardModel;
		std::unique_ptr<TBSGameRenderer> renderer;
		// GameStateRenderer renderer
	};
}
