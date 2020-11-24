#pragma once
#include <Representation/AbstractGS/GameState.h>
#include <ForwardModel/AbstractFM/Action.h>

namespace SGA
{
	class Effect
	{
	public:
		virtual ~Effect() = default;
		Effect(const Effect& other) = delete;
		Effect(Effect&& other) noexcept = delete;
		Effect& operator=(const Effect& other) = delete;
		Effect& operator=(Effect&& other) noexcept = delete;
		
		virtual void execute(GameState& state, std::vector<ActionTarget>& targets) const = 0;
	};
}