#pragma once
#include <string>
#include <unordered_set>
#include <vector>

namespace SGA
{
	struct GameState;
	struct Vector2f;
	struct Vector2i;
	struct Tile;
	
	struct SamplingMethod
	{
		std::string name;

		//Based in position
		virtual std::vector<Vector2i> getPositions(const GameState& gameState, const Vector2f& position) const = 0;
		virtual std::vector<int> getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs) const = 0;

		//Optional without position
		virtual std::vector<Vector2i> getPositions(const GameState& gameState) const ;
		virtual std::vector<int> getEntities(const GameState& gameState, const std::unordered_set<int>& entityTypeIDs) const;

		//Validate 
		virtual bool validatePosition(const GameState& gameState, const Vector2f& sourcePosition, const Vector2f& targetPosition) const;
		virtual bool validatePosition(const GameState& gameState,const Vector2f& targetPosition) const;

		virtual ~SamplingMethod()
		{

		}
	};

	struct Neighbours: public SamplingMethod
	{
		enum class ShapeType
		{
			Circle,
			Square,
			AllPositions
		};
		
		// Specific variables for targetType=Position
		ShapeType shapeType;
		int shapeSize;

		virtual std::vector<Vector2i> getPositions(const GameState& gameState, const Vector2f& position) const override;
		virtual std::vector<int> getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs) const override;
		
		//Validate 
		virtual bool validatePosition(const GameState& gameState, const Vector2f& sourcePosition, const Vector2f& targetPosition) const override;
	private:
		using SamplingMethod::getEntities;
		using SamplingMethod::getPositions;
		using SamplingMethod::validatePosition;
	};

	struct Dijkstra : public SamplingMethod
	{
		// Specific variables for targetType=Position
		int searchSize;
		bool allowDiagonals;

		struct Node
		{
			Node() = default;
			
			Node(int x, int y):
			posX(x),
			posY(y)
			{
				
			}
			int posX;
			int posY;
			bool visited = false;
			float totalCost = 0;
			float estimatedCost = 0;

			inline bool operator<(const Node& rhs) const
			{
				return totalCost+estimatedCost < rhs.totalCost+rhs.estimatedCost;
			}

			inline bool operator==(const Node& rhs) const
			{
				return posX==rhs.posX && posY==rhs.posY;
			}

			inline bool operator !=(const Node& rhs) const
			
			{
				return posX != rhs.posX || posY != rhs.posY;
			}
		};


		virtual std::vector<Vector2i> getPositions(const GameState& gameState, const Vector2f& position) const override;
		virtual std::vector<int> getEntities(const GameState& gameState, const Vector2f& position, const std::unordered_set<int>& entityTypeIDs) const override;
		
		//Validate 
		virtual bool validatePosition(const GameState& gameState, const Vector2f& sourcePosition, const Vector2f& targetPosition) const override;
	private:
		using SamplingMethod::getEntities;
		using SamplingMethod::getPositions;
		using SamplingMethod::validatePosition;
	};
}
