#include "headers.h"

namespace stratega
{
	void technology(py::module_& m)
	{
		// ---- TechnologyTreeCollection ----	
		py::class_<SGA::TechnologyTreeCollection, std::shared_ptr<SGA::TechnologyTreeCollection>>(m, "TechnologyTreeCollection", "Contains a list of technology trees, indexed by a unique ID.")
			.def_readwrite("technology_tree_types", &SGA::TechnologyTreeCollection::technologyTreeTypes, "List of technology trees, indexed by a unique ID.")
			.def("get_technology", &SGA::TechnologyTreeCollection::getTechnology, py::arg("technologyID"), "Returns a technology node with an ID corresponding to the one passed by parameter. The node could come from any of the trees of this collection.")
			.def("get_technology_type_id", &SGA::TechnologyTreeCollection::getTechnologyTypeID, py::arg("technologyName"), "Returns the ID of a technology that matches the requested name, in any of the trees of this collection.")
			;
		// ---- TechnologyTreeType ----	
		py::class_<SGA::TechnologyTreeType, std::shared_ptr<SGA::TechnologyTreeType>>(m, "TechnologyTreeType", "Contains the definition of a technology tree type.")
			.def_readwrite("technology_tree_name", &SGA::TechnologyTreeType::technologyTreeName, "Name of this technology tree")
			.def_readwrite("technologies", &SGA::TechnologyTreeType::technologies, "Data of this tech tree, each entry contains and ID as key and a technology node as a value")
			.def("get_technology_node", &SGA::TechnologyTreeType::getTechnologyNode, py::arg("technologyID"), "Returns a tech tree node from this tree with a corresponding technology ID.")
			.def("exists_technology_tree_node", &SGA::TechnologyTreeType::existsTechnologyTreeNode, py::arg("technologyID"), "Checks if a given technology ID exists in this tree.")
			.def("getTechnology_node_id", &SGA::TechnologyTreeType::getTechnologyNodeID, py::arg("technologyName"), "Returns the ID of a technology that matches the requested name.")
			.def("exists_technology_node", &SGA::TechnologyTreeType::existsTechnologyNode, py::arg("technologyName"), "Checks if a technology with a given name exists in this tree.")
			;
		// ---- TechnologyTreeNode ----	
		py::class_<SGA::TechnologyTreeNode, std::shared_ptr<SGA::TechnologyTreeNode>>(m, "TechnologyTreeNode", "Contains definition of a specific technology. It has methods to get the cost, description or name.")
			.def_readwrite("name", &SGA::TechnologyTreeNode::name, "Name of this technology as given in YAML")
			.def_readwrite("description", &SGA::TechnologyTreeNode::description, "Description of this technology (provided in YAML)")
			.def_readwrite("id", &SGA::TechnologyTreeNode::id, "Unique ID of this technology.")
			.def_readwrite("parent_ids", &SGA::TechnologyTreeNode::parentIDs, "IDs of the technologies that are required for this one to be researchable.")
			.def_readwrite("cost", &SGA::TechnologyTreeNode::cost, "List of pairs (param,cost) that can be spend to research this technology.")
			.def_readwrite("continuousActionTime", &SGA::TechnologyTreeNode::continuousActionTime, "How many ticks/turns takes to research this technology.")
			.def("to_string", &SGA::TechnologyTreeNode::toString, py::arg("gameInfo"), "Returns a string with printable information of this research technology")
			;
	}	
}