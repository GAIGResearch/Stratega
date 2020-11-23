#pragma once
#include <string>

namespace SGA
{
    class SearchSpace
    {
    private:
    	std::string _name;
        size_t _nDims;

    public:
        SearchSpace(std::string name, const size_t nDims) :_name(std::move(name)), _nDims(nDims) {};
        virtual ~SearchSpace();
    	
        [[nodiscard]] std::string getName() const { return _name; };
        [[nodiscard]] int getNumDims() const { return _nDims; };

        virtual std::vector<int> getRandomPoint();
        virtual int getSize();
        virtual int getDimSize(int j);
        virtual int getRandomValueInDim(int dim);
    };
}

