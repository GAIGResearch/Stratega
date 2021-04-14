#pragma once
#include <Stratega/Configuration/GameConfig.h>

#include "NewGameStateRenderer.h"
#include "Configuration/RenderConfig.h"

namespace SGA
{
	class GameRunner;
	
	class Trainer
	{
	public:
		void step(/*ActionAssignment*/);
		
	private:
		GameRunner& runner;
	};
	
	class GameRunner
	{
	public:

		void step(/*ActionAssignment*/);
		void reset();
		void render();
		void play(/*std::vector<Agent> agents*/);
		void createTrainer(/*agents*/);
		
	private:
		GameConfig gameConfig;
		RenderConfig renderConfig;

		GameState currentState;
		std::unique_ptr<EntityForwardModel> fm;
		std::unique_ptr<NewGameStateRenderer> renderer;
		std::unique_ptr<Trainer> trainer;
	};
}
