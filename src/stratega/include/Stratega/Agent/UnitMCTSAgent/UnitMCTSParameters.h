#pragma once
#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/TwoKingdomsAbstractionHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>
#include <Stratega/Agent/UnitMCTSAgent/Transition.h>
#include <boost/random.hpp>
#include <yaml-cpp/yaml.h>

namespace SGA {
    struct UnitMCTSParameters: AgentParameters {
        int OPPONENT_ID = -1;

        bool IS_SCSA = false;

        double K = sqrt(2);
        int ROLLOUT_LENGTH = 3;
        bool ROLLOUTS_ENABLED = true;

        bool PRIORITIZE_ROOT = true;
        double EPSILON = 1e-2;  // 1e-2;

        bool DO_STATE_ABSTRACTION = false;

        double R_THRESHOLD = 0.1; //0.1, 0.08
        double T_THRESHOLD = 0.3; // {0.5, 1.5, 2.5, 3.5}
        double TECHNOLOGY_R_THRESHOLD = 0.4;
        double COMBAT_R_THRESHOLD = 0.3;
        //double RESOURCE_R_THRESHOLD = 0.0;

        bool RANDOM_ABSTRACTION = false;

        bool CONTINUE_PREVIOUS_SEARCH = false;
        double REMAINING_FM_CALLS = -1;

        int global_nodeID = 0;

        Transition global_transition = Transition();

        int maxDepth = 0;

        // approximate homomorhpism
        int batch_size = 20;
        int n_batch_stop = 2;
        int absBatch = 10000;

        bool IS_MULTI_OBJECTIVE = false;

        bool HAS_PLAYER_ACTION = false;

        // for state abstraction according to the value;
        std::vector< double > approx_Q = {};
        std::vector< int > depth_list = {};

        bool IS_ACTION_INDEPENDENCE = false;

        boost::random::uniform_real_distribution< double >
            doubleDistribution_ = boost::random::uniform_real_distribution< double >(0, 1);

        // std::unique_ptr<StateHeuristic> STATE_HEURISTIC = nullptr;
        // std::make_unique<MinimizeDistanceHeuristic>();

        std::unique_ptr<BaseActionScript> OPPONENT_MODEL = std::make_unique<RandomActionScript>();

        /*
        * Use an independent heuristic for MDP homomorphism.
        */
        std::shared_ptr<BasicTBSCombatHeuristic> abstractionHeuristic;

        std::shared_ptr<BasicTBSCombatHeuristic> heuristic;
        //std::shared_ptr<BasicTBSTechnologyHeuristic> abstractionHeuristic;

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
            rhs.IS_MULTI_OBJECTIVE           = node["IsMultiObjective"].as< int >(rhs.IS_MULTI_OBJECTIVE);
            rhs.TECHNOLOGY_R_THRESHOLD       = node["TechnologyRThreshold"].as< double >(rhs.TECHNOLOGY_R_THRESHOLD);
            rhs.COMBAT_R_THRESHOLD           = node["CombatRThreshold"].as< double >(rhs.COMBAT_R_THRESHOLD);
            rhs.RANDOM_ABSTRACTION           = node["RandomAbstraction"].as< bool >(rhs.RANDOM_ABSTRACTION);
            rhs.R_THRESHOLD                  = node["RThreshold"].as< double >(rhs.R_THRESHOLD);
            rhs.T_THRESHOLD                  = node["TThreshold"].as< double >(rhs.T_THRESHOLD);
            rhs.IS_ACTION_INDEPENDENCE           = node["IsActionIndependence"].as< bool >(rhs.IS_ACTION_INDEPENDENCE);
            rhs.IS_SCSA           = node["IsScsa"].as< bool >(rhs.IS_SCSA);
            return true;
        }
    };
}  // namespace YAML
