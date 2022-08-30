#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ForwardModel.h>

namespace SGA
{
	template<typename NodeType>
	class ITreeNode
	{

	public:

		GameState gameState;
		NodeType* parentNode = nullptr;
		std::vector<std::unique_ptr<NodeType>> children;
		int childIndex = 0;
		double value = 0;
		int ownerID; //ID of the player that owns the tree this node belongs to.

		ITreeNode(const ForwardModel& forwardModel, const GameState& newGameState, int newOwnerID) :
			ITreeNode(forwardModel, std::move(newGameState), nullptr, 0, newOwnerID) {}
		
		ITreeNode(const ForwardModel& forwardModel, const GameState& newGameState, NodeType* parent, const int newChildIndex, int newOwnerID) :
			gameState(std::move(newGameState)), parentNode(parent), childIndex(newChildIndex), ownerID(newOwnerID)
		{
			children = std::vector<std::unique_ptr<NodeType>>();
			computeActionSpace(forwardModel);
		}
		
		virtual std::vector<Action> getActionSpace(const ForwardModel& /*forwardModel*/, int /*playerID*/) const { return actionSpace; }
		virtual ~ITreeNode() = default;
		ITreeNode(const ITreeNode& other) = delete;
		ITreeNode(ITreeNode&&) = default;
		virtual void print() const = 0;		

		void printTree() const
		{
			printTree("", this, true, "root");
		}


		virtual bool isFullyExpanded() const
		{
			return children.size() >= actionSpace.size();
		}

        /// <summary>
		/// Action space in the state of this node. This is the action space corresponding to:
		///  - The owner of this tree if ownerID can play in this state (then playerID=ownerID)
		///  - If the owner can't play at this state, the player that can play if there's only one other to play.
		/// </summary>
		std::vector<Action> actionSpace;

	protected:

		
		int playerID;  //ID of the player whose action space has been calculated in this node (and it's to move in the game).


		/// <summary>
		/// If ownerID can play, actionSpace is generated for player ownerID only.
		/// If ownerID can't play, actionSpace is genreated for the current player if only one can play.
		/// </summary>
		/// <param name="forwardModel"></param>
		void computeActionSpace(const ForwardModel& forwardModel)
		{
			if (gameState.canPlay(ownerID))
			{
				actionSpace = forwardModel.generateActions(this->gameState, ownerID);
				playerID = ownerID;
			}
			else
			{
				std::vector<int> pIDs = gameState.whoCanPlay();
				if (pIDs.size() >= 1)
				{
					actionSpace = forwardModel.generateActions(this->gameState, pIDs[0]);
					playerID = pIDs[0];
				}
			}
		}


		void printTree(const std::string& prefix, const ITreeNode<NodeType>* node, bool isLast, const std::string& actionName) const
		{
			if (node != nullptr)
			{
				if (!node->children.empty())
				{
					std::cout << prefix;

					if (isLast)
						std::cout << "\\-- ";
					else
						std::cout << "|-- ";

					// print the value of the node
					node->print();
					std::cout << actionName << std::endl;

					// enter the next tree level - left and right branch
					for (size_t i = 0; i < node->children.size(); ++i)
					{
						printTree(prefix + (isLast ? "   " : "|  "), node->children[i].get(), i == (node->children.size() - 1),
							"");
					}
				}
				else
				{
					//std::cout<<"Children Empty!" << std::endl;
					std::cout << prefix;

					if (isLast)
						std::cout << "\\-- ";
					else
						std::cout << "|-- ";

					node->print();
					//std::cout << node->children.size() << "; " << actionName << std::endl;
					std::cout << actionName << std::endl;
				}
			}
		}

	};
}