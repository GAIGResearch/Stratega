#pragma once
#include <chrono>
#include <ostream>
#include <string>

namespace SGA
{
	struct TechnologyTreeNode
	{
		std::string name;
		std::string description;
		int id;
		std::vector<int> parentIDs;
		std::unordered_map<ParameterID, double> cost;
		double continuousActionTime;
	};

	class TechnologyTreeType
	{
	public:
		static const int UNDEFINED_TECHNOLOGY_ID = -1;
		
		std::string technologyTreeName;

		//Data
		//ID + technologynode
		std::unordered_map<int, TechnologyTreeNode> technologies;

		const TechnologyTreeNode& getTechnologyNodeConst(int technologyID)const
		{
			//Search technology in tree
			const auto& it = technologies.find(technologyID);

			if (it != technologies.end())
				//We found the technology						
				return it->second;
		}
		
		bool findTechnologyNode(int technologyID)const
		{
			//Search technology in tree

			const auto& it = technologies.find(technologyID);

			if (it != technologies.end())
				//We found the technology						
				return true;
			else
				return false;
		}

		int getTechnologyNodeID(std::string technologyName)const
		{
			//Search through technologies
			for (const auto& technology : technologies)
			{
				if (technology.second.name == technologyName)
					return technology.second.id;
			}
		}

		bool haveTechnologyNode(std::string technologyName)const
		{
			//Search through technologies
			for (const auto& technology : technologies)
			{
				if (technology.second.name == technologyName)
					return true;
			}

			return false;
		}
		
	};

	class TechnologyTreeCollection
	{
	public:
		//List of tech types
		std::unordered_map<int, TechnologyTreeType> technologyTreeTypes;
		//PlayerID + ResearchedTechnologyID
		std::unordered_map<int, std::vector<int>> researchedTechnologies;

		bool isResearched(int playerID, int technologyID) const
		{
			//Search if the technology is found in the list of researchedtechnologies
			const auto& researchedPairList = researchedTechnologies.find(playerID);

			for (auto& element : researchedPairList->second)
			{
				if (element==technologyID)
					return true;
			}
			return false;
		}

		bool canResearch(int playerID, int technologyID) const
		{
			//Check if is researched
			if (isResearched(playerID, technologyID))
				return false;
			
			//Check if technology parents are researched		
			const TechnologyTreeNode& technologyNode = getTechnology(technologyID);

			const std::vector<int>& parentsIDs = technologyNode.parentIDs;

			for (auto& parent : parentsIDs)
			{
				const TechnologyTreeNode& technologyParentNode = getTechnology(parent);

				if (!isResearched(playerID, technologyParentNode.id))
				{
					std::cout << "Can't research " << technologyNode.name << std::endl;
					return false;
				}
			}

			std::cout << "Can research " << technologyNode.name << std::endl;
			return true;
		}

		void researchTechnology(int playerID, int technologyID)
		{
			//Get researched technologies of player
			const auto& researchedPairList = researchedTechnologies.find(playerID);

			std::cout << "Researched " << technologyID << std::endl;
			//Find technology index and add it to the researched list			
			researchedPairList->second.emplace_back(technologyID);
		}
		
		const TechnologyTreeNode& getTechnology(int technologyID) const
		{
			//Search through technologytreetypes
			for (const auto& technologyTreeType : technologyTreeTypes)
			{
				if(technologyTreeType.second.findTechnologyNode(technologyID))
					return technologyTreeType.second.getTechnologyNodeConst(technologyID);
			}
		}

		int getTechnologyTypeID(std::string technologyName) const
		{
			for (const auto& technologyTreeType : technologyTreeTypes)
			{
				if (technologyTreeType.second.haveTechnologyNode(technologyName))
					return technologyTreeType.second.getTechnologyNodeID(technologyName);
			}
		}
	};

}
