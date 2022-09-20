#pragma once
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSParameters.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>

namespace SGA {

   class UnitMCTSNode : public ITreeNode<UnitMCTSNode>
   {
       public:
          int nodeDepth = 0;
          int treesize = 1;
          int childExpanded = 0; // for the case one child is expanded and has been deleted
          bool recordedQ = false;
          bool isAbstracted = false;

          int unitThisStep = 0;
          std::vector<int> unitIndex;
          std::vector<int> unitStepTookCount;

          int nodeID = -1;

          std::map<int, double> actionToReward;
          std::map<int, int> actionToNextState;
          std::map<int, int> stateCounter;
          std::vector<int> actionHashVector;
          std::map<int, int> actionHashes;
          std::vector<int> nextStateHashVector;

          // MDP homomorphism
          double absValue = 0.0;
          int absVisitCount = 0;

          int nVisits = 0;
          int absNodeID = -1;

          int n_search_iteration= 0; // this is for abstraction

          // ungrouping
          bool isUngrouped = false;

       protected:
          double bounds[2] = {0, 1};// {numeric_limits<double>::min(), numeric_limits<double>::max()};

          [[nodiscard]] bool isFullyExpanded() const {
             return childExpanded >= actionSpace.size();
          }

       public:

          void initializeNode();
          void increaseTreeSize();

          // tree policy phase
          UnitMCTSNode* treePolicy(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator, std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes, std::map<int, std::vector<double> >* absNodeToStatistics);
          UnitMCTSNode* expand(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator, std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes);
          UnitMCTSNode* uct(UnitMCTSParameters& params, boost::mt19937& randomGenerator, std::map<int, std::vector<double> >* absNodeToStatistics);

          // rollout phase
          double rollOut(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator);
          static bool rolloutFinished(GameState& rollerState, int depth, UnitMCTSParameters& params);

          // backpropagation phase
          static void backUp(UnitMCTSNode* node, double result, std::map<int, std::vector<double> >* absNodeToStatistics);

          // return action
          int bestAction(UnitMCTSParameters& params, boost::mt19937& randomGenerator);

          // helper functions
          static double normalize(double aValue, double aMin, double aMax);
          static double noise(double input, double epsilon, double random);
          int applyActionToGameState(ForwardModel& forwardModel, GameState& targetGameState, Action& action, UnitMCTSParameters& params, boost::mt19937& randomGenerator) const;
          void setDepth(int depth);

          void eliminateAbstraction();
          void eliminateAbstraction(std::map<int, std::vector<double> >* absNodeToStatistics);

          // Root Node Constructor
          UnitMCTSNode(ForwardModel& forwardModel, GameState gameState, std::vector<int> unitIndex_, int unitThisStep_, int playerID, int nodeID_);

          //void setRootGameState(shared_ptr<TreeNode> root);
          void searchMCTS(ForwardModel& forwardModel, UnitMCTSParameters& params, boost::mt19937& randomGenerator, std::map<int, std::vector<UnitMCTSNode*> >* depthToNodes, std::map<int, std::vector<double> >* absNodeToStatistics);
          int mostVisitedAction(UnitMCTSParameters& params, boost::mt19937& randomGenerator);
          void print() const override;
          void get_branching_number(std::vector<int>* v, int* n);

          void printActionInfo() const;

          double getValue(std::map<int, std::vector<double> >* absNodeToStatistics);
          int getVisitCount(std::map<int, std::vector<double> >* absNodeToStatistics);
          void setValue(double result, std::map<int, std::vector<double> >* absNodeToStatistics, bool increase = false);
          void setVisitCount(double result, std::map<int, std::vector<double> >* absNodeToStatistics, bool increase = false);

       private:
          //UnitMCTSNode(TBSForwardModel& forwardModel, GameState gameState, UnitMCTSNode* parent, int childIndex);
          UnitMCTSNode(ForwardModel& forwardModel, GameState gameState, UnitMCTSNode* parent, 
              const int childIndex, std::vector<int> unitIndex_, int unitThisStep_, int playerID, int nodeID_);
      
   };
}