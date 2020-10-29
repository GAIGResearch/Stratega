#pragma once
#include <ForwardModel/ActionSpace.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSForwardModel.h>

namespace SGA
{	
	template<typename NodeType>
	class ITreeNode
	{
	public:
		TBSGameState gameState;
		NodeType* parentNode = nullptr;
		std::vector<std::unique_ptr<NodeType>> children;
		std::unique_ptr<ActionSpace<Vector2i>> actionSpace;
		int childIndex = 0;
		double value = 0;
		
	public:
		ITreeNode(TBSForwardModel* forwardModel, TBSGameState gameState) :
			ITreeNode(forwardModel, std::move(gameState), nullptr, 0)
		{
		}

	
		ITreeNode(TBSForwardModel* forwardModel, TBSGameState gameState, NodeType* parent, const int childIndex) :
			gameState(std::move(gameState)), parentNode(parent), childIndex(childIndex)
		{
			children = std::vector<std::unique_ptr<NodeType>>();
			
			actionSpace = forwardModel->getActions(this->gameState);
		}
		
		virtual ~ITreeNode() = default;
		//virtual std::string toString() const = 0;
		virtual void print() const = 0;

	protected:
		[[nodiscard]] bool isFullyExpanded() const {
			return children.size() >= actionSpace->count();
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

		ITreeNode& operator=(const ITreeNode&) = delete;
	};
}
