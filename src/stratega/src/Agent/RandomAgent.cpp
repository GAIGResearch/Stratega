#include <boost/random.hpp>
#include <Stratega/Agent/RandomAgent.h>

namespace SGA
{
	ActionAssignment RandomAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		//std::cout<<"start random Agent" << std::endl;
		// Generate all available actions
		auto actions = forwardModel.generateActions(state, getPlayerID());
		// Uniformly sample a action
		
		/*for (auto a : actions) {
			state.printActionInfo(a);
		}
		std::cout<<"End action space"<<std::endl;
		*/
		boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
		auto actionIndex = actionDist(getRNGEngine());
		auto action = actions.at(actionIndex);

		return ActionAssignment::fromSingleAction(action);
	}
}