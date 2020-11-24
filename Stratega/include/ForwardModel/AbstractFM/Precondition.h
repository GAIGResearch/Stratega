#pragma once
#include <Representation/AbstractGS/GameState.h>
#include <ForwardModel/AbstractFM/Action.h>

namespace SGA
{
	class Precondition
	{
	public:
		virtual ~Precondition() = default;
		Precondition(const Precondition& other) = delete;
		Precondition(Precondition&& other) noexcept = delete;
		Precondition& operator=(const Precondition& other) = delete;
		Precondition& operator=(Precondition&& other) noexcept = delete;
		
		virtual bool isFullfilled(const GameState& state, const Action& action) const = 0;
	};
}