#pragma once
#include <vector>
#include <memory>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Game/GameObserver.h>

namespace SGA
{
	class Agent;	

	/// <summary>
	/// An abstract class that provides a simplified interface to play games with a given <see cref="SGA::GameConfig"/>.
	/// Use "createGameRunner" to create an instance of this class.
	/// </summary>
	class GameRunner
	{
	private:
		static inline GameObserver* defaultObserver = new GameObserver();
		
	public:
		/// <summary>
		/// Initializes a new GameRunner from a given configuration file.
		/// </summary>
		/// <param name="config">The configuration file that should be used by the GameRunner. 
		/// The given configuration has to be kept in memory while this object exists.</param>
		explicit GameRunner(const GameConfig& config);
		virtual ~GameRunner() = default;

		/// <summary>
		/// Resets the game to an initial state.
		/// </summary>
		void reset();

		/// <summary>
		/// Resets the game to an initial state with a specific map.
		/// </summary>
		void reset(int levelID);
		
		/// <summary>
		/// Advances the game by one timestep.
		/// When the game has ended, you are responsible for calling <see cref="SGA::GameRunner::reset()"> to reset the environments state.
		/// </summary>
		/// <param name="actions">The action used to advance the game. Depending on the game type, the action is validated differently.</param>
		void step(const ActionAssignment& actions);
		
		/// <summary>
		/// Renders a visual representation of the game. May create a new window when called for the first time.
		/// </summary>
		void render();
		
		/// <summary>
		/// Advances the game until it has ended.
		/// </summary>
		/// <param name="agents">A list of agents to obtain actions from.</param>
		/// <param name="observer">An optional observer who is notified about important events in the game.</param>
		void run(std::vector<std::shared_ptr<Agent>>& agents, GameObserver* observer = nullptr);
		
		/// <summary>
		/// Renders a visual representation of the game and allows interactive action selection.
		/// </summary>
		/// <param name="agents">A list of agents to obtain actions from. Exactly one agent can be left empty, which is 
		/// then controlled by the human player.</param>
		void play(std::vector<std::shared_ptr<Agent>>& agents);
		
		/// <summary>
		/// Returns a reference to the current state of the game.
		/// </summary>
		[[nodiscard]] const GameState& getGameState() const;

	protected:
		void initializeAgents(std::vector<std::shared_ptr<Agent>>& agents);
		void ensureRendererInitialized();

		virtual void runInternal(std::vector<std::shared_ptr<Agent>>& agents, GameObserver& observer) = 0;
		virtual void playInternal(std::vector<std::shared_ptr<Agent>>& agents, int humanIndex) = 0;
		
		virtual void checkInitializationTime(std::chrono::milliseconds initializationTime, int playerID);

		std::unique_ptr<ForwardModel> forwardModel;
		std::unique_ptr<GameRenderer> renderer;
		std::unique_ptr<GameState> currentState;
		const GameConfig* config;

		//Computation budget time
		bool shouldCheckComputationTime;		
		long budgetTimeMs;
		long disqualificationBudgetTimeMs;
		std::vector<int> playerWarnings;
		int maxNumberWarnings;

		//Intialization budget time
		bool shouldCheckInitTime;
		long initBudgetTimetMs;
		long initDisqualificationBudgetTimeMs;

	};

	/// <summary>
	/// Initializes a new GameRunner suited for the given configuration file.
	/// </summary>
	/// <param name="config">The configuration file that should be used by the GameRunner. The given configuration has to 
	/// be kept in memory while this object exists.</param>
	/// <returns>A pointer to the initialized GameRunner.</returns>
	std::unique_ptr<GameRunner> createGameRunner(const GameConfig& config);
}


