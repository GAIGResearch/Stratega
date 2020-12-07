#pragma once
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <Representation/AbstractGS/RTSGameState2.h>
#include <Representation/AbstractGS/Player.h>
namespace  SGA
{
	class RTSAbstractForwardModel : public ForwardModel<RTSGameState2>
	{
	public:
		void advanceGameState(RTSGameState2& state, const Action& action) const override
		{
			//Execute the action
			action.execute(state, *this);

			//Update removed entities
			/*for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i -= 1;
				}
			}*/

			//Check game is finished
			state.isGameOver = checkGameIsFinished(state);
		}

		
	};
}
