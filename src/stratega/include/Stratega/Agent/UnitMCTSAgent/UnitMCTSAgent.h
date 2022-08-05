#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSNode.h>

#include "UnitMCTSParameters.h"

namespace SGA {

    class UnitMCTSAgent: public Agent {
    public:

        std::map< int, std::vector< UnitMCTSNode* > > depthToNodes;

        std::vector< std::vector< std::vector< UnitMCTSNode* > > >
            absNodes;  // first dimension: depth, second dimension: absnode Index, second
        std::map< int, std::vector< double > >
            absNodeToStatistics;  // 1st, 2nd, and 3rd dimension of absNodes as key, pair of <value,
                                // visitCount> as value
        int global_absNodeIndex = 0;
        std::map< int, int > treeNodetoAbsNode;

        bool newRound = true;

        void init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/);

        explicit UnitMCTSAgent(const std::string& name, UnitMCTSParameters&& params) : Agent{ name }, parameters_(std::move(params))
        {
            // K=0.1, RL=1, OS=3;

            // parameters_.ROLLOUT_LENGTH = 1;
            // parameters_.OPPONENT_MODEL = nullptr;		// the opponent model has not been correctly
            // set in the NTBEA evaluation
            unitIndex = std::vector< int >();

            stepInit();
        }

        void stepInit()
        {
            depthToNodes = std::map< int, std::vector< UnitMCTSNode* > >();

            absNodes = std::vector< std::vector< std::vector< UnitMCTSNode* > > >();
            for(int i = 0; i < 100; i++) {
                absNodes.push_back(std::vector< std::vector< UnitMCTSNode* > >());
            }

            absNodeToStatistics = std::map< int, std::vector< double > >();
            treeNodetoAbsNode = std::map< int, int >();
        }

        void deleteAbstraction()
        {
            // initialize the abstraction-related data as empty
            absNodes = std::vector< std::vector< std::vector< UnitMCTSNode* > > >();
            for(int i = 0; i < 100; i++) {
                absNodes.push_back(std::vector< std::vector< UnitMCTSNode* > >());
            }

            absNodeToStatistics = std::map< int, std::vector< double > >();
            treeNodetoAbsNode = std::map< int, int >();
        }

        //ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, long timeBudgetMs) override;
        ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
        bool isTwoNodeApproxmateHomomorphism(
            const ForwardModel& forwardModel,
            UnitMCTSNode* node1,
            UnitMCTSNode* node2,
            double reward_threshold,
            double transition_threshold);
        bool isHomomorphismMultiObjective(
            const ForwardModel& forwardModel,
            UnitMCTSNode* node1,
            UnitMCTSNode* node2);

        std::vector< Action > switchUnit(GameState state, const ForwardModel& forwardModel);
        void printBoundStatistics();

private:
        std::unique_ptr< UnitMCTSNode > rootNode = nullptr;
        UnitMCTSParameters parameters_;

        bool initialized = false;
        int previousActionIndex = -1;
        bool continuePreviousSearch = true;
        int playerTurn = -1;
        int step = 1;

        bool unitIndexInitialized = false;
        int unitThisStep = 0;  // this is an array index of unitIndex
        int unitNextStep = 0;

        /*
        * at the beginning of each round, the unit informatino is stored
        */
        std::vector< int > unitIndex;

        /*
        * recording quantities to calculate loss bound
        */
        std::vector<double> nD_min_list, h_size_list, a_size_list;
        bool isPrintedLossBound = false;
        const int nLossBoundStep = 20;


    };
}  // namespace SGA
