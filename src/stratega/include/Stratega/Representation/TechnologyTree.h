#pragma once
#include <chrono>
#include <ostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace SGA
{
	struct GameInfo;
	typedef int ParameterID;

	struct TechnologyTreeNode
	{
		/// <summary>
		/// Name of this technology as given in YAML
		/// </summary>
		std::string name;

		//Description of this technology (provided in YAML)
		std::string description;

		/// <summary>
		/// Unique ID of this technology.
		/// </summary>
		int id = -1;

		/// <summary>
		/// IDs of the technologies that are required for this one to be researchable.
		/// </summary>
		std::vector<int> parentIDs;

		/// <summary>
		/// List of pairs (param,cost) that can be spend to research this technology.
		/// </summary>
		std::unordered_map<ParameterID, double> cost;

		/// <summary>
		/// How many ticks/turns takes to research this technology.
		/// </summary>
		double continuousActionTime = 0;

		/// <summary>
		/// Returns a string with printable information of this research technology
		/// </summary>
		/// <param name="gameInfo">Reference to the game information object of this game.</param>
		/// <returns>The printable string of this technology.</returns>
		std::string toString(const GameInfo& gameInfo) const;
	};

	class TechnologyTreeType
	{
	public:
		static const int UNDEFINED_TECHNOLOGY_ID = -1;
		
		/// <summary>
		/// Name of this technology tree
		/// </summary>
		std::string technologyTreeName;

		/// <summary>
		/// Data of this tech tree, each entry contains and ID as key and a technology node as a value
		/// </summary>
		std::unordered_map<int, TechnologyTreeNode> technologies;

		/// <summary>
		/// Returns a tech tree node from this tree with a corresponding technology ID.
		/// </summary>
		/// <param name="technologyID">ID of the technology we are looking for</param>
		/// <returns>Technology node to retrieve from this tree.</returns>
		const TechnologyTreeNode& getTechnologyNode(int technologyID)const;
		
		/// <summary>
		/// Checks if a given technology ID exists in this tree.
		/// </summary>
		/// <param name="technologyID">Tech ID to check</param>
		/// <returns>True if there's a technology in this tree with the ID received.</returns>
		bool existsTechnologyTreeNode(int technologyID)const;

		/// <summary>
		/// Returns the ID of a technology that matches the requested name.
		/// </summary>
		/// <param name="technologyName">Name to match of a technology in the tree.</param>
		/// <returns>The ID of the technology with the same name as the one passed.</returns>
		int getTechnologyNodeID(const std::string& technologyName)const;

		/// <summary>
		/// Checks if a technology with a given name exists in this tree.
		/// </summary>
		/// <param name="technologyName">Name of the technology to check</param>
		/// <returns>True if it exists.</returns>
		bool existsTechnologyNode(const std::string& technologyName)const;
		
	};

	class TechnologyTreeCollection
	{
	public:
		
		/// <summary>
		/// List of technology trees, indexed by a unique ID.
		/// </summary>
		std::unordered_map<int, TechnologyTreeType> technologyTreeTypes;		
		
		/// <summary>
		/// Returns a technology node with an ID corresponding to the one passed by parameter. The node could come from
		/// any of the trees of this collection.
		/// </summary>
		/// <param name="technologyID">ID of the technology to search for.</param>
		/// <returns>The tree node searched for, an exception if it doesn't exist.</returns>
		const TechnologyTreeNode& getTechnology(int technologyID) const;

		/// <summary>
		/// Returns the ID of a technology that matches the requested name, in any of the trees of this collection.
		/// </summary>
		/// <param name="technologyName">Name of the technology to search for.</param>
		/// <returns>ID of the technology searched for, or a runtime error if it doesn't exist.</returns>
		int getTechnologyTypeID(const std::string& technologyName) const;
	};

}
