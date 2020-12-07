#pragma once
#include <ForwardModel/AbstractFM/ForwardModel.h>
#include <Representation/AbstractGS/TBSGameState2.h>
namespace  SGA
{
	class TBSAbstractForwardModel: public ForwardModel<TBSGameState2>
	{
	public:
		void advanceGameState(TBSGameState2& state, const Action& action) const override
		{
			//Execute the action
			action.execute(state);

			//Update removed entities
			for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i -= 1;
				}
			}

			//Check game is finished
			state.isGameOver = checkGameIsFinished(state);
		}
		
	};
}
