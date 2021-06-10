#pragma once
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/Agent/StateAbstraction/AbstractState.h>

namespace SGA
{
	template<typename NodeType>
	class IAbstractTreeNode
	{
	public:
		GameState gameState;
		AbstractState abstractGameState;
		int equivalenceClassID = -1;
		int nrOfEquivalenceClasses = 0;

		NodeType* parentNode = nullptr;
		int previousAction = -1;
		
		std::vector<std::unique_ptr<NodeType>> children;
		
		std::vector<Action> actionSpace;
		int childIndex = 0;
		double value = 0;

	public:
		IAbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState) :
			IAbstractTreeNode(forwardModel, std::move(abstractState), std::move(gameState), nullptr, 0)
		{
		}


		IAbstractTreeNode(TBSForwardModel& forwardModel, AbstractState abstractState, GameState gameState, NodeType* parent, const int childIndex) :
			 gameState(gameState), abstractGameState(std::move(abstractState)), parentNode(parent), childIndex(childIndex)
		{
			children = std::vector<std::unique_ptr<NodeType>>();

			actionSpace = forwardModel.generateActions(gameState);
		}

		virtual ~IAbstractTreeNode() = default;
		//virtual std::string toString() const = 0;
		virtual void print() const = 0;

	//todo make these protected after all tests are done
	public:
		[[nodiscard]] bool isFullyExpanded() const {
			return children.size() >= actionSpace.size();
		}

		void printTree(const std::string& prefix, const IAbstractTreeNode<NodeType>* node, bool isLast, const std::string& actionName) const
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
					std::cout << prefix;

					if (isLast)
						std::cout << "\\-- ";
					else
						std::cout << "|-- ";

					std::cout << node->children.size() << "; " << actionName << std::endl;
				}
			}
		}

	public:
		void printTree() const
		{
			printTree("", this, true, "root");
		};

		IAbstractTreeNode& operator=(const IAbstractTreeNode&) = delete;
	};
}
