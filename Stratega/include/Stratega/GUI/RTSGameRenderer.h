#pragma once
#include <Stratega/GUI/GameRenderer.h>

namespace SGA
{
	class RTSGameRenderer : public GameRenderer
	{
	public:

		void init(const GameState& initialState, const GameConfig& gameConfig) override {};
		void update(const GameState& state) override {};
		void render() override;
		void setPlayerPointOfView(int playerID) override {};
		ActionAssignment getPlayerActions() override { return ActionAssignment{}; };
	};
}
