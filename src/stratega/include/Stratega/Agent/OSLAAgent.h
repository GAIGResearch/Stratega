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
		boost::uniform_real<double> doubleDistribution_ = boost::uniform_real<double>(0, 1);  //Uniform distribution of real numbers in [0,1]

		double epsilon = 1e-2; //Small number to avoid /0
	};
}