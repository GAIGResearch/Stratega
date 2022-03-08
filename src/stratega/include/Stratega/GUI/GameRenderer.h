#pragma once
#include <Stratega/ForwardModel/ActionAssignment.h>
#include <memory>
#include <Stratega/Utils/warnings.h>

DISABLE_WARNING_PUSH
#if defined(__clang__)    
	DISABLE_WARNING_UNREFERENCED
#elif defined(__GNUC__)
	DISABLE_WARNING_UNREFERENCED
#endif

namespace SGA
{
	enum class GameType;
	struct RenderConfig;
	struct GameConfig;
	struct GameState;

	static const int NO_PLAYER_ID = -2;

	/// <summary>
	/// An abstract renderer that renders game states and provides interactive action selection.
	/// Use "createRenderer" to create an instance of this class.
	/// </summary>
	class GameRenderer
	{
	public:
		
		
		virtual ~GameRenderer() = default;

		/// <summary>
		/// Initializes the renderer, has to be called first before calling <see cref="SGA::GameRenderer::render()">.
		/// Note that the window for rendering may already be created before calling this method.
		/// </summary>
		/// <param name="initialState">The initial state that should be displayed in the renderer.</param>
		/// <param name="gameConfig">The configuration file that should be used by the GameRenderer. The given configuration has to be kept in memory while this object exists.</param>
		virtual void init(const GameState& initialState, const GameConfig& gameConfig) = 0;
		/// <summary>
		/// Updates the game state that is displayed.
		/// </summary>
		/// <param name="state">The new state that should be displayed in the renderer.</param>
		virtual void update(const GameState& state) = 0;
		/// <summary>
		/// Handles input and updates the window. This method has to be called continuously, or else the window will freeze.
		/// </summary>
		virtual void render() = 0;
		/// <summary>
		/// Sets the point of view from which the user can see and interact with the game.
		/// </summary>
		/// <param name="playerID">The player ID which is controlled by the player, if the ID is -1 the user is treated as a spectator.</param>
		virtual void setPlayerPointOfView(int playerID) = 0;
		/// <summary>
		/// Returns the actions that the user has selected. The returned actions will always be valid for the currently displayed game state.
		/// But it may happen that the user hasn't selected an action yet, resulting in invalid action.
		/// For example, in TBS games, the user has to select one action; selecting no action is not valid.
		/// </summary>
		/// <returns>The ActionAssignment containing all actions selected by the user. May be empty if no action was selected, or if the user is a spectator.</returns>
		virtual ActionAssignment getPlayerActions() = 0;
		/// <summary>
		/// Returns a boolean indicating that the user has requested to end the game, for example, by trying to close the window.
		/// Note that the renderer will ignore any of these attempts. You have to manually destroy this renderer to fulfill the user's request.
		/// </summary>
		/// <returns>True if the user wants to end the game, otherwise false.</returns>
		virtual bool isGameEndRequested() = 0;
		/// <summary>
		/// Close the sfml window.
		/// </summary>
		virtual void closeWindow() = 0;

		virtual bool isWaiting() const = 0;
	};

	/// <summary>
	/// Initializes a new GameRenderer suited for the given configuration file.
	/// </summary>
	/// <param name="type">The type of the game that you want to render.</param>
	/// <returns>A pointer to the initialized GameRenderer.</returns>
	std::unique_ptr<GameRenderer> createRenderer(const GameType& type, SGA::Vector2i& resolution);
}
