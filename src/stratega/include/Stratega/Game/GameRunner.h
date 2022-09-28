#pragma once
#include <vector>
#include <memory>
#include <cassert>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <Stratega/Configuration/GameConfig.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Game/GameObserver.h>
#include <Stratega/Utils/IteratorToPtrVector.h>

namespace SGA
{
	class Agent;	

	/// <summary>
	/// An abstract class that provides a simplified interface to play games with a given <see cref="SGA::GameConfig"/>.
	/// Use "createGameRunner" to create an instance of this class.
	/// </summary>
	static GameObserver* defaultObserver = new GameObserver();
	class GameRunner
	{
	private:
		
		
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
		virtual void reset();

		/// <summary>
		/// Resets the game to an initial state with a specific map.
		/// </summary>
		virtual void reset(int levelID);
		
		/// <summary>
		/// Advances the game by one timestep.
		/// When the game has ended, you are responsible for calling <see cref="SGA::GameRunner::reset()"> to reset the environments state.
		/// </summary>
		/// <param name="actions">The action used to advance the game. Depending on the game type, the action is validated differently.</param>
		virtual void step(const ActionAssignment& actions);
		
		/// <summary>
		/// Renders a visual representation of the game. May create a new window when called for the first time.
		/// </summary>
		virtual void render(Vector2i& resolution);
		
		/// <summary>
		/// Advances the game until it has ended.
		/// </summary>
		/// <param name="begin">The beginning of the range containing the agents to obtain actions from</param>
		/// <param name="end">The end of the range containing the agents to obtain actions from</param>
		/// <param name="observer">An optional observer who is notified about important events in the game.</param>
		template<typename Iterator>
		void run(Iterator begin, Iterator end, GameObserver* observer = nullptr)
		{
			assert(std::distance(begin, end) == static_cast<int>(currentState->getPlayers().size()));
			if (observer == nullptr)
			{
				observer = defaultObserver;
			}

			auto agents = IteratorToPtrVector<Iterator, Agent>::convert(begin, end);
			try
			{
				// Check that no player is controlled by a human
				for (size_t i = 0; i < agents.size(); i++)
				{
					if (agents[i] == nullptr)
					{
                        std::cout<<"AgentID: "<< i << "\n";
						throw std::runtime_error("No player can be controlled by a human in an arena.");
					}
					else
					{
						agents[i]->setPlayerID(static_cast<int>(i));
					}
				}

				initializeAgents(agents);
				observer->onGameStarted(*currentState, *forwardModel);
				runInternal(agents, *observer);
				observer->onGameFinished(*currentState, *forwardModel);
			}
			catch (const std::exception& ex)
			{
				std::cout << "GameRunner crashed error: " << ex.what() << std::endl;
			}
		}
		
		/// <summary>
		/// Renders a visual representation of the game and allows interactive action selection.
		/// </summary>
		/// <param name="begin">The beginning of a range containing agents to obtain actions from. Exactly one agent can be left empty, which is 
		/// then controlled by the human player.</param>
		/// <param name="end">The end of the range containing the agents to obtain actions from.</param>
		/// <param name="resolution">The resolution of the window that should be used to display the game.</param>
		template<typename Iterator>
		void play(Iterator begin, Iterator end, Vector2i& resolution)
		{
			assert(std::distance(begin, end) == static_cast<int>(currentState->getPlayers().size()));

			auto agents = IteratorToPtrVector<Iterator, Agent>::convert(begin, end);
			try
			{
				// Check if a player is controlled by an human
				int humanIndex = NO_PLAYER_ID;
				for (int i = 0; i < static_cast<int>(agents.size()); i++)
				{
					if (agents[i] == nullptr)
					{
						if (humanIndex != NO_PLAYER_ID)
						{
							throw std::runtime_error("Only one player can be controlled by a human. Index " + std::to_string(humanIndex) + " is already empty.");
						}
						humanIndex = i;
					}
					else
					{
						agents[i]->setPlayerID(i);
					}
				}

				initializeAgents(agents);
				ensureRendererInitialized(resolution);
				initializeRenderer(humanIndex);
				playInternal(agents, humanIndex);
			}
			catch (const std::exception& ex)
			{
				std::cout << "GameRunner crashed error: " << ex.what() << std::endl;
			}
		}
		
		/// <summary>
		/// Returns a reference to the current state of the game.
		/// </summary>
		const GameState& getGameState() const;

	protected:
		virtual void initializeAgents(std::vector<Agent*>& agents);
		virtual void ensureRendererInitialized(Vector2i& resolution);

		virtual void runInternal(std::vector<Agent*>& agents, GameObserver& observer) = 0;
		virtual void playInternal(std::vector<Agent*>& agents, int humanIndex) = 0;
		
		virtual void checkInitializationTime(std::chrono::milliseconds initializationTime, int playerID);

		virtual void initializeRenderer(int humanIndex)
		{
			renderer->setPlayerPointOfView(humanIndex);
		}

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


