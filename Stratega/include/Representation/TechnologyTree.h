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

		//TechnologyTreeCollection()
		//{
		//	//Test
		//	//Create treetype
		//	TechnologyTreeType buildingTechnologyTreeType;
		//	buildingTechnologyTreeType.technologyTreeName = "Building";

		//	TechnologyTreeNode townHall;
		//	townHall.id = 0;
		//	townHall.description = "This is a townhall";
		//	townHall.name = "TownHall";

		//	TechnologyTreeNode archery;
		//	archery.id = 1;
		//	archery.description = "This is a archery";
		//	archery.name = "Archery";
		//	archery.parentIDs.emplace_back(0);

		//	TechnologyTreeNode advancedArchery;
		//	advancedArchery.id = 2;
		//	advancedArchery.description = "This is a advanced archery";
		//	advancedArchery.name = "Advanced Archery";
		//	advancedArchery.parentIDs.emplace_back(1);
		//	advancedArchery.parentIDs.emplace_back(4);

		//	TechnologyTreeNode moreAdvancedArchery;
		//	moreAdvancedArchery.id = 3;
		//	moreAdvancedArchery.description = "This is a more advanced archery";
		//	moreAdvancedArchery.name = "More Advanced Archery";
		//	moreAdvancedArchery.parentIDs.emplace_back(2);

		//	TechnologyTreeNode goodBows;
		//	goodBows.id = 4;
		//	goodBows.description = "You need this before researching advanced archery";
		//	goodBows.name = "Good Bows";
		//	goodBows.parentIDs.emplace_back(0);

		//	buildingTechnologyTreeType.nodes.emplace_back(townHall);
		//	buildingTechnologyTreeType.nodes.emplace_back(archery);
		//	buildingTechnologyTreeType.nodes.emplace_back(advancedArchery);
		//	buildingTechnologyTreeType.nodes.emplace_back(moreAdvancedArchery);
		//	buildingTechnologyTreeType.nodes.emplace_back(goodBows);

		//	buildingTechnologyTreeType.technologies["TownHall"] = 0;
		//	buildingTechnologyTreeType.technologies["Archery"] = 1;
		//	buildingTechnologyTreeType.technologies["Advanced Archery"] = 2;
		//	buildingTechnologyTreeType.technologies["More Advanced Archery"] = 3;
		//	buildingTechnologyTreeType.technologies["Good Bows"] = 4;
		//	
		//	technologyTreeTypes[0]= buildingTechnologyTreeType;

		//	//Player 0 and 1 init
		//	researchedTechnologies[0] = {};
		//	researchedTechnologies[1] = {};

		//	/*int numberExecutions = 100000;
		//	auto t1 = std::chrono::high_resolution_clock::now();
		//	for (size_t i = 0; i < numberExecutions; i++)
		//	{
		//		isResearched(0, 0, 1);
		//	}
		//	auto t2 = std::chrono::high_resolution_clock::now();

		//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		//	std::cout << duration/numberExecutions<<std::endl;*/

		//	canResearch(0, 0, 1);
		//	research(0, 0, 0);

		//	canResearch(0, 0, 1);
		//	research(0, 0, 1);

		//	canResearch(0, 0, 2);
		//	canResearch(0, 0, 4);
		//	research(0, 0, 4);			
		//	canResearch(0, 0, 2);
		//	research(0, 0, 2);

		//}
		//TODO search by int 
		//List of tech types
		std::unordered_map<int, TechnologyTreeType> technologyTreeTypes;
		//PlayerID + pair of technology tree type id + the researched technology
		std::unordered_map<int, std::vector<std::pair<int, int>>> researchedTechnologies;

		bool isResearched(int playerID, int techologyTreeTypeName, int technologyName)
		{
			//Search if the technology is found in the list of researchedtechnologies
			auto& researchedPairList = researchedTechnologies.find(playerID);
			for (auto& element : researchedPairList->second)
			{
				if (element.first == techologyTreeTypeName && element.second == technologyName)
					return true;
			}
			return false;
		}

		bool canResearch(int playerID, int techologyTreeTypeName, int technologyName)
		{
			//Check if technology parents are researched
			//Get tech tree type
			TechnologyTreeType& technologyTreeType = technologyTreeTypes[techologyTreeTypeName];

			TechnologyTreeNode& technologyNode = technologyTreeType.searchNode(technologyName);

			std::vector<int>& parentsIDs = technologyNode.parentIDs;

			for (auto& parent : parentsIDs)
			{
				TechnologyTreeNode& technologyParentNode = technologyTreeType.searchNode(parent);

				//TODO search by int 
				if (!isResearched(playerID, techologyTreeTypeName, technologyParentNode.id))
				{
					std::cout << "Cant research " << technologyNode.name << std::endl;
					return false;
				}
			}

			std::cout << "Can research " << technologyNode.name << std::endl;
			return true;
		}

		void  research(int playerID, int techologyTreeTypeName, int technologyName)
		{
			//Get researched technologies of player
			auto& researchedPairList = researchedTechnologies.find(playerID);

			std::cout << "Researched " << technologyName << std::endl;
			//Find technology index and add it to the researched list			
			researchedPairList->second.emplace_back(techologyTreeTypeName, technologyName);
		}
		
		//bool isResearched(int playerID, std::string techologyTreeTypeName,std::string technologyName)
		//{
		//	//Search if the technology is found in the list of researchedtechnologies
		//	auto& researchedPairList = researchedTechnologies.find(playerID);
		//	for (auto& element : researchedPairList->second)
		//	{
		//		if (element.first == techologyTreeTypeName && element.second == technologyName)
		//			return true;

		//	}
		//	return false;
		//}

		

		//bool canResearch(int playerID, std::string techologyTreeTypeName, std::string technologyName)
		//{
		//	//Check if technology parents are researched
		//	
		//	//Get tech tree type
		//	TechnologyTreeType& technologyTreeType = technologyTreeTypes[techologyTreeTypeName];

		//	TechnologyTreeNode& technologyNode=technologyTreeType.searchNode(technologyName);

		//	std::vector<int>& parentsIDs=technologyNode.parentIDs;

		//	for (auto& parent : parentsIDs)
		//	{

		//		TechnologyTreeNode& technologyParentNode = technologyTreeType.searchNode(parent);

		//		//TODO search by int 
		//		if (!isResearched(playerID, techologyTreeTypeName,technologyParentNode.name))
		//		{
		//			//std::cout << "Cant research " << technologyName<<std::endl;
		//			return false;
		//		}
		//			
		//	}

		//	//std::cout << "Can research " << technologyName << std::endl;
		//	return true;
		//}

		

		//void  research(int playerID, std::string techologyTreeTypeName, std::string technologyName)
		//{
		//	//Search if the technology is found in the list of researchedtechnologies
		//	auto& researchedPairList = researchedTechnologies.find(playerID);


		//	//Get tech tree type
		//	TechnologyTreeType& technologyTreeType = technologyTreeTypes[techologyTreeTypeName];

		//	TechnologyTreeNode& technologyNode = technologyTreeType.searchNode(technologyName);

		//	std::cout << "Researched " << technologyName << std::endl;
		//	//Find technology index and add it to the researched list			
		//	researchedPairList->second.emplace_back(techologyTreeTypeName, technologyName);
		//}

		
	};

}
