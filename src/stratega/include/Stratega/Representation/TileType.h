#pragma once
#include <Stratega/Representation/Tile.h>
#include <Stratega/Representation/Parameter.h>
namespace SGA
{
	struct TileType
	{

	private:

		/// <summary>
		/// Name of the tile as given in YAML>
		/// </summary>
		std::string name;

		/// <summary>
		/// Symbol of this tile as used in YAML for level placement.
		/// </summary>
		char symbol = '\0';

		/// <summary>
		/// Unique ID for this tile type.
		/// </summary>
		int id = -1;

		/// <summary>
		/// Indicates if the tile is walkable by units
		/// </summary>
		bool walkable = true;

		/// <summary>
		/// Indicates if this tile blocks the line of sight
		/// </summary>
		bool blocksSight = false;

		/// <summary>
		/// Indicates if this is a default tile. Default tiles are used for fog of war (to substitute
		///  elements that should be hidden) and to be place in positions where units are defined in the YAML level layout.
		/// </summary>
		bool defaultTile = false;

		/// <summary>
		/// Map of parameters of this tile. Key is an integer (ID of the parameter) and value is the paramter object.
		/// The parameter defines the min, max and default value this parameter can take.
		/// </summary>
		std::unordered_map<ParameterID, Parameter> parameters;


	public:

		/// <summary>
		/// Creates an instantiation of this tile given a position
		/// </summary>
		/// <param name="x">X position of the tile</param>
		/// <param name="y">Y position of the tile</param>
		/// <returns>A Tile object of this type in the position <x,y></returns>
		Tile toTile(int x, int y) const
		{
			Tile instance(this, x, y);
			instance.setWalkable(walkable);
			instance.setBlockSight(blocksSight);
			return instance;
		}

		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameter(ParameterID id) const;

		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameterByIndex(int index) const;

		/// <summary>
		/// Returns a <see cref="SGA::Parameter"/> reference that contains the parameter defintion
		/// </summary>
		const Parameter& getParameterByName(std::string& name) const;

		/// <summary>
		/// Returns the maximum value of a given parameter
		/// </summary>
		double getParamMax(std::string paramName) const;

		/// <summary>
		/// Returns the minimum value of a given parameter
		/// </summary>
		double getParamMin(std::string paramName) const;

		/* Getters and setters for this type */
		const std::string& getName() const { return name; }
		void setName(const std::string& newName) { this->name = newName; }

		char getSymbol() const { return symbol; }
		void setSymbol(char c) { symbol = c; }

		int getID() const { return id; }
		void setID(int newID) { this->id = newID; }

		bool isWalkable() const { return walkable; }
		void setWalkable(bool w) { walkable = w; }

		bool blockSight() const { return blocksSight; }
		void setBlockSight(bool b) { blocksSight = b; }

		bool isDefaultTile() const { return defaultTile; }
		void setDefaultTile(bool newDefaultTile) { this->defaultTile = newDefaultTile; }

		const std::unordered_map<ParameterID, Parameter>& getParameters() const { return parameters; }
		std::unordered_map<ParameterID, Parameter>& getParameters() { return parameters; }
	};
}
