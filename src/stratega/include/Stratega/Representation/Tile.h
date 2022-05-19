#pragma once
#include <Stratega/Representation/Vector2.h>
#include <unordered_map>
#include <vector>
#pragma warning( disable : 4820)
namespace SGA
{
	struct TileType;
	
	struct Tile
	{

	private:

		/// <summary>
		/// Type of tile
		/// </summary>
		const TileType* type;

		/// <summary>
		/// Indicates if this tile blocks the line of sight for entities in the game. 
		/// </summary>
		bool blockSight;

		/// <summary>
		/// Indicates if this tile is walkable for units to occupy it.
		/// </summary>
		bool walkable;

		/// <summary>
		/// Position <x,y> of this tile in the board.
		/// </summary>
		Vector2i position;

		/// <summary>
		/// Values for the parameters of this tile. Indexed by ID. Use getParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> parameters;

		/// <summary>
		/// Values for the max parameters value of this tile. Indexed by ID. Use getMaxParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> maxParameters;

		/// <summary>
		/// Values for the min parameters value of this tile. Indexed by ID. Use getMinParameter(...) functions to access these.
		/// </summary>
		std::unordered_map<int, double> minParameters;

	public:

		Tile(const TileType* newTileType, int x, int y);

		/// <summary>
		/// Returns the tile type ID of this tile
		/// </summary>
		/// <returns>type ID of this tile</returns>
		int getTileTypeID() const;

		/// <summary>
		/// Returns the tile type of this tile
		/// </summary>
		/// <returns>type of this tile</returns>
		const TileType& getTileType() const
		{
			return *type;
		}
		
		/// <summary>
		/// Returns the name of this tile type
		/// </summary>
		/// <returns>Name ot the tile type of this type.</returns>
		const std::string name() const;
		
		/*Getters and setters for Tile*/

		inline bool isWalkable() const { return walkable; }
		inline void setWalkable(bool w) { walkable = w; }

		inline bool blocksSight() const { return blockSight; }
		inline void setBlockSight(bool b) { blockSight = b; }

		inline void setPosition(int x, int y) { position = Vector2i(x,y); }
		inline int x() const { return position.x; }
		inline int y() const { return position.y; }

		/// <summary>
		/// Gets the list of parameters values. Modifiable.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		std::unordered_map<int, double>& getParamValues() { return parameters; }

		/// <summary>
		/// Gets the list of parameters values.
		/// <summary>
		/// <returns>The list of parameters values.</returns>
		const std::unordered_map<int, double>& getParamValues() const { return parameters; }

		/// <summary>
		/// Gets the list of parameters names.
		/// <summary>
		/// <returns>The list of parameters names of this tile.</returns>
		std::vector<std::string> getTileParameterNames() const;

		/// <summary>
		/// Gets a map with all pairs <parameter,value>
		/// <summary>
		/// <returns>Returns a map with all the parameters.</returns>
		std::unordered_map<std::string, double> getTileParameters() const;

		/// <summary>
		/// Gets the value of a specific parameter with buffs applied, by name 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double getParameter(const std::string& paramName) const;

		/// <summary>
		/// Gets the reference value of a specific parameter raw, by name 
		/// <summary>
		/// <returns>The parameter reference value.</returns>
		double& getRawParameter(const std::string& paramName);

		/// <summary>
		/// Gets a specific parameters raw, by index 
		/// <summary>
		/// <returns>The parameter reference value.</returns>
		double& getRawParameterAt(int paramIdx) { return parameters[paramIdx]; }

		/// <summary>
		/// Gets a specific parameters value, by index 
		/// <summary>
		/// <returns>The parameter value.</returns>
		double& getParameterAt(int paramIdx) { return parameters[paramIdx]; }

		/// <summary>
		/// Gets a specific max parameters value, by index 
		/// <summary>
		/// <returns>The max parameter value.</returns>
		double getMaxParameterAt(int paramIdx) { return maxParameters[paramIdx]; }

		/// <summary>
		/// Gets a specific min parameters value, by index 
		/// <summary>
		/// <returns>The min parameter value.</returns>
		double getMinParameterAt(int paramIdx) { return minParameters[paramIdx]; }
	};

}