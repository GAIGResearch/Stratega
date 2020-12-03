#pragma once
#include <string>
#include <iostream>

#include <vector>

#include <SearchSpace.h>

namespace SGA
{
    /// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class Evaluator
    {
    protected:
        std::string _name;
        std::unique_ptr<SearchSpace> _searchSpace;

    public:
        Evaluator(std::string name, std::unique_ptr<SearchSpace> searchSpace) :
    		_name(std::move(name)), _searchSpace(std::move(searchSpace))
        {
	        
        }
    	
        virtual ~Evaluator() = default;
    	
        [[nodiscard]] std::string getName() const { return _name; }
        [[nodiscard]] SearchSpace* getSearchSpace() const { return _searchSpace.get(); }
    	
        virtual std::vector<float> evaluate(std::vector<int> point, int nSamples) = 0;
        virtual void printPoint(const std::vector<int>& point) = 0;
    };
    

}

    
