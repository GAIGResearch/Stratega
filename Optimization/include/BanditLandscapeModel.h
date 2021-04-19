#pragma once
#include <string>
#include <vector>

namespace SGA
{
    class BanditLandscapeModel
    {
    private:
        std::string _name = "BanditLandscapeModel";
    	
    public:
        BanditLandscapeModel(std::string&& name) : _name(std::move(name)) {};
    	virtual ~BanditLandscapeModel() = default;
    	
        virtual void reset() = 0;
        virtual void init() = 0;
        virtual void addEvaluatedPoint(std::vector<int>& point, double fitness) = 0;
        virtual double getMeanEstimate(const std::vector<int>& point) = 0;
        virtual double getExplorationEstimate(std::vector<int>& point) = 0;
        virtual std::pair<std::vector<int>, double> getBestSampled() = 0;
    	
        [[nodiscard]] std::string getName() const { return _name; };
    };
}
