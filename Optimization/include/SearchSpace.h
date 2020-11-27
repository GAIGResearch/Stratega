#pragma once
#include <string>
#include <vector>

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

        virtual std::vector<int> getRandomPoint() = 0;
        virtual int getSize() = 0;
        virtual int getDimSize(int j) = 0;
        virtual int getRandomValueInDim(int dim) = 0;
        
    };
    
}

