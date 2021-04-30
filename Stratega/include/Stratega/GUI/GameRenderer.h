#pragma once
#include <Stratega/ForwardModel/ActionAssignment.h>

namespace SGA
{
	enum class GameType;
	struct RenderConfig;
	struct GameConfig;
	struct GameState;

	class GameRenderer
	{
	public:
		static inline int NO_PLAYER_ID = -1;
		
		virtual ~GameRenderer() = default;

		virtual void init(const GameState& initialState, const GameConfig& gameConfig) = 0;
		virtual void update(const GameState& state) = 0;
		virtual void render() = 0;
		virtual void setPlayerPointOfView(int playerID) = 0;
		virtual ActionAssignment getPlayerActions() = 0;
	};

	std::unique_ptr<GameRenderer> createRenderer(GameType type);
}
