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
	};

	class TechnologyTreeType
	{
	public:
		std::string technologyTreeName;

		//Data
		//Name of Technology + index
		std::unordered_map<std::string, int> technologies;
		//List of treenodes sorted by index/id
		std::vector<TechnologyTreeNode> nodes;
	public:
		TechnologyTreeNode& searchNode(std::string technologyName)
		{
			//Search first index of the technology
			auto it = technologies.find(technologyName);
			int techIndex = it->second;

			return nodes[techIndex];
		}

		TechnologyTreeNode& searchNode(int technologyID)
		{
			return nodes[technologyID];
		}
	};

	class TechnologyTreeCollection
	{
	public:

		TechnologyTreeCollection()
		{
			//Test
			//Create treetype
			//TechnologyTreeType buildingTechnologyTreeType;
			//buildingTechnologyTreeType.technologyTreeName = "Building";

			//TechnologyTreeNode townHall;
			//townHall.id = 0;
			//townHall.description = "This is a townhall";
			//townHall.name = "TownHall";

			//TechnologyTreeNode archery;
			//archery.id = 1;
			//archery.description = "This is a archery";
			//archery.name = "Archery";
			//archery.parentIDs.emplace_back(0);

			//TechnologyTreeNode advancedArchery;
			//advancedArchery.id = 2;
			//advancedArchery.description = "This is a advanced archery";
			//advancedArchery.name = "Advanced Archery";
			//advancedArchery.parentIDs.emplace_back(1);
			//advancedArchery.parentIDs.emplace_back(4);

			//TechnologyTreeNode moreAdvancedArchery;
			//moreAdvancedArchery.id = 3;
			//moreAdvancedArchery.description = "This is a more advanced archery";
			//moreAdvancedArchery.name = "More Advanced Archery";
			//moreAdvancedArchery.parentIDs.emplace_back(2);

			//TechnologyTreeNode goodBows;
			//goodBows.id = 4;
			//goodBows.description = "You need this before researching advanced archery";
			//goodBows.name = "Good Bows";
			//goodBows.parentIDs.emplace_back(0);

			//buildingTechnologyTreeType.nodes.emplace_back(townHall);
			//buildingTechnologyTreeType.nodes.emplace_back(archery);
			//buildingTechnologyTreeType.nodes.emplace_back(advancedArchery);
			//buildingTechnologyTreeType.nodes.emplace_back(moreAdvancedArchery);
			//buildingTechnologyTreeType.nodes.emplace_back(goodBows);

			//buildingTechnologyTreeType.technologies["TownHall"] = 0;
			//buildingTechnologyTreeType.technologies["Archery"] = 1;
			//buildingTechnologyTreeType.technologies["Advanced Archery"] = 2;
			//buildingTechnologyTreeType.technologies["More Advanced Archery"] = 3;
			//buildingTechnologyTreeType.technologies["Good Bows"] = 4;
			//
			//technologyTreeTypes[0]= buildingTechnologyTreeType;

			////Player 0 and 1 init
			//researchedTechnologies[0] = {};
			//researchedTechnologies[1] = {};

			/*int numberExecutions = 100000;
			auto t1 = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < numberExecutions; i++)
			{
				isResearched(0, 0, 1);
			}
			auto t2 = std::chrono::high_resolution_clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

			std::cout << duration/numberExecutions<<std::endl;*/

			//canResearch(0, 0, 1);
			//research(0, 0, 0);

			/*canResearch(0, 0, 1);
			research(0, 0, 1);

			canResearch(0, 0, 2);
			canResearch(0, 0, 4);
			research(0, 0, 4);
			canResearch(0, 0, 2);
			research(0, 0, 2);*/

		}

		//List of tech types
		std::unordered_map<int, TechnologyTreeType> technologyTreeTypes;
		//PlayerID + pair of technology tree type id + the researched technology
		std::unordered_map<int, std::vector<std::pair<int, int>>> researchedTechnologies;

		int getTechnologyTreeTypeID(int technologyID) const
		{
			for (auto& technologyTreeType : technologyTreeTypes)
			{
				for (auto& technology : technologyTreeType.second.nodes)
				{
					if (technology.id == technologyID)
					{
						//We found the technology						
						return technologyTreeType.first;
					}
				}
			}
		}

		bool isResearched(int playerID, int techologyTreeID, int technologyID) const
		{
			//Search if the technology is found in the list of researchedtechnologies
			auto& researchedPairList = researchedTechnologies.find(playerID);
			for (auto& element : researchedPairList->second)
			{
				if (element.first == techologyTreeID && element.second == technologyID)
					return true;
			}
			return false;
		}

		bool isResearched(int playerID, int technologyID) const
		{
			//Search if the technology is found in the list of researchedtechnologies
			auto& researchedPairList = researchedTechnologies.find(playerID);

			for (auto& element : researchedPairList->second)
			{
				if (element.second == technologyID)
					return true;
			}
			return false;
		}

		bool canResearch(int playerID, int techologyTreeID, int technologyID)
		{
			//Check if technology parents are researched
			//Get tech tree type
			TechnologyTreeType& technologyTreeType = technologyTreeTypes[techologyTreeID];

			TechnologyTreeNode& technologyNode = technologyTreeType.searchNode(technologyID);

			std::vector<int>& parentsIDs = technologyNode.parentIDs;

			for (auto& parent : parentsIDs)
			{
				TechnologyTreeNode& technologyParentNode = technologyTreeType.searchNode(parent);

				//TODO search by int 
				if (!isResearched(playerID, techologyTreeID, technologyParentNode.id))
				{
					std::cout << "Cant research " << technologyNode.name << std::endl;
					return false;
				}
			}

			std::cout << "Can research " << technologyNode.name << std::endl;
			return true;
		}

		bool canResearch(int playerID, int technologyID) const
		{
			//Check if is researched
			if (isResearched(playerID, technologyID))
				return false;
			
			//Check if technology parents are researched
			//Get tech tree type
			int techTreeID = getTechnologyTreeTypeID(technologyID);
			TechnologyTreeType technologyTreeType = technologyTreeTypes.at(techTreeID);

			TechnologyTreeNode& technologyNode = technologyTreeType.searchNode(technologyID);

			std::vector<int>& parentsIDs = technologyNode.parentIDs;

			for (auto& parent : parentsIDs)
			{
				TechnologyTreeNode& technologyParentNode = technologyTreeType.searchNode(parent);

				//TODO search by int 
				if (!isResearched(playerID, technologyParentNode.id))
				{
					std::cout << "Cant research " << technologyNode.name << std::endl;
					return false;
				}
			}

			std::cout << "Can research " << technologyNode.name << std::endl;
			return true;
		}

		void  research(int playerID,int technologyID)
		{
			//Get researched technologies of player
			auto& researchedPairList = researchedTechnologies.find(playerID);

			//Get tech tree type
			int techTreeID = getTechnologyTreeTypeID(technologyID);
			
			std::cout << "Researched " << technologyID << std::endl;
			//Find technology index and add it to the researched list			
			researchedPairList->second.emplace_back(techTreeID, technologyID);
		}
		
		//void  research(int playerID, int techologyTreeID, int technologyID)
		//{
		//	//Get researched technologies of player
		//	auto& researchedPairList = researchedTechnologies.find(playerID);

		//	std::cout << "Researched " << technologyID << std::endl;
		//	//Find technology index and add it to the researched list			
		//	researchedPairList->second.emplace_back(techologyTreeID, technologyID);
		//}

		const TechnologyTreeNode& getTechnologyType(int technologyID)const
		{
			for (const auto& technologyTreeType : technologyTreeTypes)
			{
				for (const auto& technology : technologyTreeType.second.nodes)
				{
					if (technology.id == technologyID)
						return technology;
				}
			}
		}

		int getTechnologyTypeID(std::string technologyName)const
		{
			for (const auto& technologyTreeType : technologyTreeTypes)
			{				
				for (const auto& technology : technologyTreeType.second.nodes)
				{
					if (technology.name== technologyName)
						return technology.id;
				}
			}
		}
	};

}
