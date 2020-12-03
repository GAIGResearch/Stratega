#pragma once
#include <string>
#include <vector>
#include <random>

namespace SGA
{
    
    class SearchSpace
    {
    private:
    	std::string _name;
        size_t _nDims;

    public:
        SearchSpace(std::string name, const size_t nDims) :_name(std::move(name)), _nDims(nDims) {};
        virtual ~SearchSpace() = default;

    	
        [[nodiscard]] std::string getName() const { return _name; };
        [[nodiscard]] int getNumDims() const { return _nDims; };

        virtual long long getSize() = 0;
        virtual int getDimSize(int j) = 0;

        virtual std::vector<int> getRandomPoint(std::mt19937& randomGenerator) = 0;
        virtual int getRandomValueInDim(int dim, std::mt19937& randomGenerator) = 0;
        
    };
    
}

