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
		int id = -1;
		std::vector<int> parentIDs;
		std::unordered_map<ParameterID, double> cost;
		double continuousActionTime = 0;
	};

	class TechnologyTreeType
	{
	public:
		static const int UNDEFINED_TECHNOLOGY_ID = -1;
		
		std::string technologyTreeName;

		//Data
		//ID + technologynode
		std::unordered_map<int, TechnologyTreeNode> technologies;

		[[nodiscard]] const TechnologyTreeNode& getTechnologyNode(int technologyID)const
		{
			//Search technology in tree
			const auto& it = technologies.find(technologyID);
			if (it != technologies.end())
				//We found the technology						
				return it->second;

			throw std::runtime_error("Tried accessing technology with unknown id " + std::to_string(technologyID));
		}
		
		[[nodiscard]] bool findTechnologyNode(int technologyID)const
		{
			//Search technology in tree

			const auto& it = technologies.find(technologyID);

			if (it != technologies.end())
				//We found the technology						
				return true;
			else
				return false;
		}

		[[nodiscard]] int getTechnologyNodeID(const std::string& technologyName)const
		{
			//Search through technologies
			for (const auto& technology : technologies)
			{
				if (technology.second.name == technologyName)
					return technology.second.id;
			}

			throw std::runtime_error("Tried accessing technology with unknown name " + technologyName);
		}

		[[nodiscard]] bool haveTechnologyNode(const std::string& technologyName)const
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
		
		[[nodiscard]] const TechnologyTreeNode& getTechnology(int technologyID) const
		{
			//Search through technologytreetypes
			for (const auto& technologyTreeType : technologyTreeTypes)
			{
				if(technologyTreeType.second.findTechnologyNode(technologyID))
					return technologyTreeType.second.getTechnologyNode(technologyID);
			}

			throw std::runtime_error("Tried accessing technology tree with unknown id " + std::to_string(technologyID));
		}

		[[nodiscard]] int getTechnologyTypeID(const std::string& technologyName) const
		{
			for (const auto& technologyTreeType : technologyTreeTypes)
			{
				if (technologyTreeType.second.haveTechnologyNode(technologyName))
					return technologyTreeType.second.getTechnologyNodeID(technologyName);
			}

			throw std::runtime_error("Tried accessing technology with unknown name " + technologyName);
		}
	};

}
