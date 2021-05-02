#pragma once
#include <vector>
#include <memory>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/ForwardModel/EntityForwardModel.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Representation/GameState.h>

namespace SGA
{
	class Agent;
	
	class GameRunner
	{
	public:
		explicit GameRunner(const GameConfig& config);
		virtual ~GameRunner() = default;
		
		void reset();
		void step(const ActionAssignment& actions);
		void render();
		void run(std::vector<std::unique_ptr<Agent>>& agents);
		void play(std::vector<std::unique_ptr<Agent>>& agents);

		[[nodiscard]] const GameState& getGameState() const;

	protected:
		void ensureRendererInitialized();

		virtual void runInternal(std::vector<std::unique_ptr<Agent>>& agents) = 0;
		virtual void playInternal(std::vector<std::unique_ptr<Agent>>& agents, int humanIndex) = 0;
		
		std::unique_ptr<EntityForwardModel> forwardModel;
		std::unique_ptr<GameRenderer> renderer;
		std::unique_ptr<GameState> currentState;
		const GameConfig* config;
	};

	std::unique_ptr<GameRunner> createGameRunner(const GameConfig& config);
}
