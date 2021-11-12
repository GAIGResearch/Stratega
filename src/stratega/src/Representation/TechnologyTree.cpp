
#include <Stratega/Representation/TechnologyTree.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{
	std::string TechnologyTreeNode::toString(const GameInfo& gameInfo) const
	{
		std::string str = name + ": " + description + " (id: " + std::to_string(id) + ")";
		str = str + "\n\t Costs: ";
		for (auto pair : cost)
		{
			std::string currName = gameInfo.getPlayerParameter(pair.first).getName();
			str = str + currName + ": " + std::to_string(pair.second) + "; ";
		}
		/*for (const auto& [id, c] : cost)
		{
			std::string name = gameInfo.getPlayerParameter(id).getName();
			str = str + name + ": " + std::to_string(c) + "; ";
		}*/
		str = str + "\n\t Tech requirements (IDs): ";
		if (parentIDs.size() == 0)
			str = str + "None.";
		else for (const auto& currentId : parentIDs)
		{
			str = str + std::to_string(currentId) + "; ";
		}
		str = str + "\n\t Research time: " + std::to_string(continuousActionTime) + "\n";
		return str;
	}


	const TechnologyTreeNode& TechnologyTreeType::getTechnologyNode(int technologyID)const
	{
		//Search technology in tree
		const auto& it = technologies.find(technologyID);
		if (it != technologies.end())
			//We found the technology						
			return it->second;

		throw std::runtime_error("Tried accessing technology with unknown id " + std::to_string(technologyID));
	}

	bool TechnologyTreeType::existsTechnologyTreeNode(int technologyID)const
	{
		//Search technology in tree
		const auto& it = technologies.find(technologyID);
		return it != technologies.end();
	}

	int TechnologyTreeType::getTechnologyNodeID(const std::string& technologyName)const
	{
		//Search through technologies
		for (const auto& technology : technologies)
		{
			if (technology.second.name == technologyName)
				return technology.second.id;
		}

		throw std::runtime_error("Tried accessing technology with unknown name " + technologyName);
	}

	bool TechnologyTreeType::existsTechnologyNode(const std::string& technologyName)const
	{
		//Search through technologies
		for (const auto& technology : technologies)
		{
			if (technology.second.name == technologyName)
				return true;
		}

		return false;
	}

	const TechnologyTreeNode& TechnologyTreeCollection::getTechnology(int technologyID) const
	{
		//Search through technologytreetypes
		for (const auto& technologyTreeType : technologyTreeTypes)
		{
			if (technologyTreeType.second.existsTechnologyTreeNode(technologyID))
				return technologyTreeType.second.getTechnologyNode(technologyID);
		}

		throw std::runtime_error("Tried accessing technology tree with unknown id " + std::to_string(technologyID));
	}

	int TechnologyTreeCollection::getTechnologyTypeID(const std::string& technologyName) const
	{
		for (const auto& technologyTreeType : technologyTreeTypes)
		{
			if (technologyTreeType.second.existsTechnologyNode(technologyName))
				return technologyTreeType.second.getTechnologyNodeID(technologyName);
		}

		throw std::runtime_error("Tried accessing technology with unknown name " + technologyName);
	}

}