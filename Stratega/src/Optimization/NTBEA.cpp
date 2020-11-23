#include <set>
#include <numeric>
#include <Optimization/NTBEA.h>


namespace SGA
{
    NTBEA::NTBEA(NTupleLandscapeModel tupleLandscape, Evaluator* evaluator, SearchSpace* searchSpace, Mutator mutator,
        int kExplore, int nSamples, int evalNeighbours) :
        _tupleLandscape(std::move(tupleLandscape)), _evaluator(evaluator), _searchSpace(searchSpace),
        _mutator(std::move(mutator)), _kExplore(kExplore), _nSamples(nSamples)
    {
        std::cout << "NTupleEvolutionaryAlgorithm";

        this->_evalNeighbors = std::min(evalNeighbours, _searchSpace->getSize());

        std::cout << "Search Space: " << _searchSpace->getName() << std::endl;
        std::cout << "Evaluator: " << _evaluator->getName() << std::endl;
        std::cout << "Mutator: " << _mutator.getName() << std::endl;
    }

    std::vector<int> NTBEA::evaluateLandscape(std::vector<int> point)
    {
        std::cout << "Estimating landscape around: " << std::endl;
        _evaluator->printPoint(point);
    	
        auto evaluated_neighbours = std::set<std::vector<int>>();

        int unique_neighbours = 0;
        float current_best_ucb = 0;
        std::vector<int> current_best_neighbour;

        // Loop until we have the required numbers of estimated neighbours
        while (unique_neighbours < _evalNeighbors)
        {
            auto potential_neighbor = _mutator.mutate(point);

            // If we already have estimates for this unique neighbour then just get a new one
            if (evaluated_neighbours.contains(potential_neighbor))
                continue;

            unique_neighbours += 1;
            const float exploit = _tupleLandscape.getMeanEstimate(potential_neighbor);
            const float explore = _tupleLandscape.getExplorationEstimate(potential_neighbor);

            const float ucb_with_noise = exploit + _kExplore * explore + rand() * _tieBreakNoise;

            evaluated_neighbours.emplace(potential_neighbor);

            std::cout << "Neighbour UCB " << ucb_with_noise << " at " << potential_neighbor << std::endl;

            // Track the best neighbor that we have seen so far
            if (current_best_ucb < ucb_with_noise)
            {
                std::cout << "Found best UCB " << ucb_with_noise << " at " << potential_neighbor << std::endl;
                current_best_ucb = ucb_with_noise;
                current_best_neighbour = potential_neighbor;
            }
        }
        return current_best_neighbour;
    }

    void NTBEA::run(int n_evaluations)
    {
        _tupleLandscape.init();

        // Get a random point to start
        auto point = _searchSpace->getRandomPoint();

        // Repeat many times
        for (int eval = 0; eval < n_evaluations; eval++)
        {
            // Explore the neighborhood in the tuple landscape and find a strong next candidate point
            // Skip this if this is the first iteration of the algorithm because it will be a random point
            if (eval > 0)
            {
                point = evaluateLandscape(point);
            }
        	
            // Evaluate the point(is repeated several times if n_samples > 0)
            std::vector<float> fitnessValues = _evaluator->evaluate(point, _nSamples);
            double fitness = 0.0f;
            const int n = fitnessValues.size();
            if (n != 0) {
                fitness = std::accumulate(fitnessValues.begin(), fitnessValues.end(), 0.0) / n;
            }
        	
            std::cout << "Evaluated fitness: " << fitness << " at [" ;
            _evaluator->printPoint(point);
        	
            // Add the new point to the tuple landscape
            _tupleLandscape.addEvaluatedPoint(point, fitness);

            if (eval % 10 == 0)
            {
                const auto solution = _tupleLandscape.getBestSampled();
                const auto bestFitness = solution.second;
                std::cout << "Iterations: " << eval << ", Best fitness: " << bestFitness << ", Solution: ";
                _evaluator->printPoint(solution.first);
                std::cout << std::endl;
            }
        }
    	
        // Once we have evaluated all the points, get the best one we have seen so far
        const auto solution = _tupleLandscape.getBestSampled();

        std::cout << "Best solution: ";
        _evaluator->printPoint(solution.first);
    	std::cout << std::endl;
	}

}
