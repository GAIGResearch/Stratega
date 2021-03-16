#include <Stratega/Agent/Heuristic/SimpleHeuristic.h>


namespace SGA {

	SimpleHeuristic::SimpleHeuristic(const int playerID, const int numberOfPlayers) :
	playerID(playerID), numberOfPlayers(numberOfPlayers)
	{
	}

	SimpleHeuristic::~SimpleHeuristic()
	{
	}

	double SimpleHeuristic::evaluateGameState(const TBSForwardModel& forwardModel, GameState& gameState, const int playerID)
	{
		std::vector<double> scores = std::vector<double>(numberOfPlayers);
		double myScore = 0.0;
		
		StatSummary othersScore{};
		for (int i = 0; i < numberOfPlayers; ++i)
		{
			scores[i] = score(forwardModel, gameState, i);
			if (i == playerID)
				myScore = scores[i];
			else
				othersScore.add(scores[i]);
		}	
		
		double scoreDiff = myScore - othersScore.getMean();
		double visCount = calculateGridVisiblePercentage(gameState);

		return scoreDiff + visCount * 10;
	}

	int SimpleHeuristic::score(const TBSForwardModel& forwardModel, GameState& gameState, int playerToScore) const
	{
		auto actions = forwardModel.generateActions(gameState);
		const int numAvailableActions = actions.size();

		const int score = gameState.getPlayer(playerToScore)->score;
		const int numberUnits = gameState.getPlayerEntities(playerToScore).size();

		int boost = 0;
		if (gameState.isGameOver) {
			if (gameState.winnerPlayerID == playerID)
				boost = WIN_BOOST;
			else
				boost = LOOSE_BOOST;
		}
		
		return numberUnits * 10 + boost + numAvailableActions + score;
	}

	double SimpleHeuristic::calculateGridVisiblePercentage(GameState& gameState) const
	{
		// todo: does the fogOfWar need to be applied by the player?
		auto copy(gameState);
		//copy.applyFogOfWar(playerID);
		int visCount = 0;
		for (size_t x = 0; x < copy.board.getWidth(); x++)
		{
			for (size_t y = 0; y < copy.board.getHeight(); y++)
			{
				if (copy.board.get(x,y).tileTypeID != copy.fogOfWarTile.tileTypeID)
					visCount++;
			}
		}

		return (double) visCount / (copy.board.getWidth() * copy.board.getHeight());
	}
}
