#pragma once
#include <string>
#include <iostream>

#include <vector>

namespace SGA
{
    /// <summary>
    /// Base class to be inherited by other evaluator objects
    /// </summary>
    class Evaluator
    {
    private:
        std::string _name;

    public:
        Evaluator(std::string name) : _name(std::move(name)) {};
        virtual ~Evaluator() = default;
    	
        [[nodiscard]] std::string getName() const { return _name; };
        virtual std::vector<float> evaluate(std::vector<int> point, int nSamples) = 0;
        virtual void printPoint(const std::vector<int>& point) = 0;
    };
    

}

    
