#include <NTBEA.h>


namespace SGA
{
    
    NTBEA::NTBEA(BanditLandscapeModel* tupleLandscape, Evaluator* evaluator, SearchSpace* searchSpace, Mutator* mutator,
        int kExplore, int nSamples, int evalNeighbors, int seed) :
        _tupleLandscape(tupleLandscape), _evaluator(evaluator), _searchSpace(searchSpace),
        _mutator(mutator), _kExplore(kExplore), _nSamples(nSamples)
    {
    	
        std::cout << "NTupleEvolutionaryAlgorithm" << std::endl;

        this->_evalNeighbors = std::min(static_cast<long long>(evalNeighbors), _searchSpace->getSize());
        std::cout << "Search Space: " << _searchSpace->getName() << std::endl;
        std::cout << "Evaluator: " << _evaluator->getName() << std::endl;
        std::cout << "Mutator: " << _mutator->getName() << std::endl;
    }

	
    std::vector<int> NTBEA::evaluateLandscape(std::vector<int>& point, std::mt19937& randomGenerator)
    {
        //SGA::LoggingScope scope("Estimating landscape around: ");
        std::cout << "Estimating landscape around: ";
        SGA::Log::logValue("Estimating landscape around: ", _evaluator->printPoint(point));       
        std::cout << std::endl;
    	
        auto evaluated_neighbors = std::set<std::vector<int>>();

        int unique_neighbors = 0;
        double current_best_ucb = 0;
        std::vector<int> current_best_neighbor;

        // Loop until we have the required numbers of estimated neighbors
        while (unique_neighbors < _evalNeighbors)
        {
            auto potential_neighbor = _mutator->mutate(point, randomGenerator);

            // If we already have estimates for this unique neighbor then just get a new one
            if (evaluated_neighbors.contains(potential_neighbor))
                continue;

            unique_neighbors += 1;
            const double exploit = _tupleLandscape->getMeanEstimate(potential_neighbor);
            const double explore = _tupleLandscape->getExplorationEstimate(potential_neighbor);

            const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
            const double ucb_with_noise = exploit + _kExplore * explore + doubleDistribution_(randomGenerator) * _tieBreakNoise;

            evaluated_neighbors.emplace(potential_neighbor);

            std::cout << "Neighbor UCB " << ucb_with_noise << " at ";
            SGA::Log::logValue("Neighbor UCB "+std::to_string(ucb_with_noise)+ " at ", _evaluator->printPoint(potential_neighbor));
            std::cout << std::endl;

            // Track the best neighbor that we have seen so far
            if (current_best_ucb < ucb_with_noise)
            {
                std::cout << "Found best UCB " << ucb_with_noise << " at ";
                SGA::Log::logValue("Found best UCB "+std::to_string(ucb_with_noise) + " at ", _evaluator->printPoint(potential_neighbor));
            	std::cout << std::endl;
                current_best_ucb = ucb_with_noise;
                current_best_neighbor = potential_neighbor;
            }
        }
        return current_best_neighbor;
    }

    void NTBEA::run(int n_evaluations, std::mt19937& randomGenerator)
    {
        try
        {
            _tupleLandscape->init();
            SGA::Log::getDefaultLogger().flush();

            // Get a random point to start
            auto point = _searchSpace->getRandomPoint(randomGenerator);

            // Repeat many times
            for (int eval = 0; eval < n_evaluations; eval++)
            {
                //std::cout << "Evaluation: " << eval << std::endl;
                // Explore the neighborhood in the tuple landscape and find a strong next candidate point
                // Skip this if this is the first iteration of the algorithm because it will be a random point
                if (eval > 0)
                {
                    point = evaluateLandscape(point, randomGenerator);
                }

                // Evaluate the point(is repeated several times if n_samples > 0)
                std::vector<float> fitnessValues = _evaluator->evaluate(point, _nSamples);
                int nrOfWins = fitnessValues.back();
                fitnessValues.pop_back();

                double fitness = 0.0f;
                const int n = fitnessValues.size();
                if (n != 0) {
                    fitness = std::accumulate(fitnessValues.begin(), fitnessValues.end(), 0.0) / n;
                }



                std::cout << "Evaluated fitness: " << fitness << " and Wins: " << nrOfWins << " at ";
                SGA::Log::logSingleValue("Evaluated fitness: " + std::to_string(fitness) + " and Wins: " + std::to_string(nrOfWins) + " at ", _evaluator->printPoint(point));
                std::cout << std::endl;

                // Add the new point to the tuple landscape
                _tupleLandscape->addEvaluatedPoint(point, fitness);

                if (eval % 10 == 0)
                {
                    const auto solution = _tupleLandscape->getBestSampled();
                    const auto bestFitness = solution.second;
                    std::cout << "Iterations: " << eval << ", Best fitness: " << bestFitness << ", Solution: ";
                    SGA::Log::logSingleValue("Iterations: " + std::to_string(eval) + ", Best fitness: " + std::to_string(bestFitness) + ", Solution: ", _evaluator->printPoint(solution.first));
                    std::cout << std::endl;
                }
                SGA::Log::getDefaultLogger().flush();
            }

            // Once we have evaluated all the points, get the best one we have seen so far
            const auto solution = _tupleLandscape->getBestSampled();

            std::cout << "Best solution: ";
            SGA::LoggingScope scope("Best solution: ");
            SGA::Log::logSingleValue("Point", _evaluator->printPoint(solution.first));
            std::cout << std::endl;
            SGA::Log::getDefaultLogger().flush();
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what();
            SGA::Log::logValue("Error", ex.what());

            SGA::Log::getDefaultLogger().flush();

            std::cout << "Best solution: ";

            SGA::LoggingScope scope("Best solution: ");
            const auto solution = _tupleLandscape->getBestSampled();
            SGA::Log::logSingleValue("Point", _evaluator->printPoint(solution.first));

            std::cout << std::endl;
            SGA::Log::getDefaultLogger().flush();
        }
        
	}
    
}
