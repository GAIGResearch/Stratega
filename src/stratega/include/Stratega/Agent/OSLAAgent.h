#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		using Agent::Agent;
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;		

	private:
		double noise(const double input, const double epsilon, const double random);
		std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);  //Uniform distribution of real numbers in [0,1]
		double epsilon = 1e-2; //Small number to avoid /0
	};
}