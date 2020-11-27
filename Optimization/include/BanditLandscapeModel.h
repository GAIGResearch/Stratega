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
    	
        virtual void reset();
        virtual void init();
        virtual void addEvaluatedPoint(std::vector<int>& point, const double fitness);
        virtual double getMeanEstimate(const std::vector<int>& point);
        virtual double getExplorationEstimate(std::vector<int>& point);

        [[nodiscard]] std::string getName() const { return _name; };

    	virtual std::pair<std::vector<int>, double> getBestSampled();
    };
    

}
