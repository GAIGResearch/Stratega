#pragma once
#include <Representation/AbstractGS/GameState.h>
#include <ForwardModel/AbstractFM/Action.h>

namespace SGA
{
	class Precondition
	{
	public:
		Precondition()
		{
			
		}
		virtual ~Precondition() = default;
		Precondition(const Precondition& other) = default;
		Precondition(Precondition&& other) noexcept = delete;
		Precondition& operator=(const Precondition& other) = delete;
		Precondition& operator=(Precondition&& other) noexcept = delete;
	
		virtual bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const = 0;
	};
}