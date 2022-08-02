#pragma once
#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>
#include <Stratega/Agent/UnitMCTSAgent/Transition.h>
#include <boost/random.hpp>
#include <yaml-cpp/yaml.h>

namespace SGA {
    struct UnitMCTSParameters: AgentParameters {
        int OPPONENT_ID = -1;

        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;

        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;  // 1e-2;

        bool DO_STATE_ABSTRACTION = false;

        double R_THRESHOLD = 0.1; //0.1, 0.08
        double T_THRESHOLD = 0.3;

        bool CONTINUE_PREVIOUS_SEARCH = true;
        double REMAINING_FM_CALLS = -1;

        int global_nodeID = 0;

        Transition global_transition = Transition();

        int maxDepth = 0;

        // approximate homomorhpism
        int batch_size = 20;
        int n_batch_stop = 2;
        int absBatch = 10000;

        // for state abstraction according to the value;
        std::vector< double > approx_Q = {};
        std::vector< int > depth_list = {};

        boost::random::uniform_real_distribution< double >
            doubleDistribution_ = boost::random::uniform_real_distribution< double >(0, 1);

        // std::unique_ptr<StateHeuristic> STATE_HEURISTIC = nullptr;
        // std::make_unique<MinimizeDistanceHeuristic>();

        std::unique_ptr<BaseActionScript> OPPONENT_MODEL = std::make_unique<RandomActionScript>();

        /*
        * Use an independent heuristic for MDP homomorphism.
        */
        std::shared_ptr<StateHeuristic> abstractionHeuristic;

        void printDetails() const;
    };
}  // namespace SGA

namespace YAML {
    template <>
    struct convert< SGA::UnitMCTSParameters > {
        static bool decode(const Node& node, SGA::UnitMCTSParameters& rhs)
        {
            rhs.decode(node);
            rhs.K                            = node["K"].as< double >(rhs.K);
            rhs.ROLLOUT_LENGTH               = node["RolloutLength"].as< int >(rhs.ROLLOUT_LENGTH);
            rhs.ROLLOUTS_ENABLED             = node["EnableRollouts"].as< bool >(rhs.ROLLOUTS_ENABLED);
            rhs.REMAINING_FM_CALLS           = rhs.maxFMCalls;
            rhs.DO_STATE_ABSTRACTION         = node["DoStateAbstraction"].as< bool >(rhs.DO_STATE_ABSTRACTION);
            rhs.R_THRESHOLD                  = node["RThreshold"].as< double >(rhs.R_THRESHOLD);
            rhs.CONTINUE_PREVIOUS_SEARCH     = node["ContinuePreviousSearch"].as< bool >(
                rhs.CONTINUE_PREVIOUS_SEARCH);
            rhs.absBatch                     = node["AbstractionBatch"].as< int >(rhs.absBatch);
            return true;
        }
    };
}  // namespace YAML
