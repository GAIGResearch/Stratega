#pragma once
#include <string>

namespace SGA
{
    class BanditLandscapeModel
    {
    private:
        std::string _name = "BanditLandscapeModel";
    public:
        BanditLandscapeModel(std::string name) : _name(name) {};
        virtual ~BanditLandscapeModel();
    	
        virtual void reset();
        virtual void init();
        virtual void addEvaluatedPoint(std::vector<int> point, double fitness);
        virtual double getMeanEstimate(std::vector<int> point);
        virtual double getExplorationEstimate(std::vector<int> point);

        [[nodiscard]] std::string getName() const { return _name; };
    };
}
