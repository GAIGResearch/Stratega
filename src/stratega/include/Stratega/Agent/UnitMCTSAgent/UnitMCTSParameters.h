#pragma once
#include <Stratega/Agent/AgentParameters.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/Heuristic/PushThemAllHeuristic.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
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
        double T_THRESHOLD = 1.0;

        bool CONTINUE_PREVIOUS_SEARCH = false;
        double REMAINING_FM_CALLS = -1;

        int global_nodeID = 0;

        Transition global_transition = Transition();

        int maxDepth = 0;

        // approximate homomorhpism
        int batch_size = 20;
        int absBatch = 10000;

		// random node clusterring
		bool random_abstraction = false;

        // for state abstraction according to the value;
        std::vector< double > approx_Q = {};
        std::vector< int > depth_list = {};

        boost::random::uniform_real_distribution< double >
            doubleDistribution_ = boost::random::uniform_real_distribution< double >(0, 1);

        // std::unique_ptr<StateHeuristic> STATE_HEURISTIC = nullptr;
        // std::make_unique<MinimizeDistanceHeuristic>();

        std::unique_ptr<BaseActionScript> OPPONENT_MODEL = std::make_unique<RandomActionScript>();

        // for ungrouping
        bool IS_UNGROUPING = false;
        bool IS_PHI_UNGROUPING = false;
        double UNGROUPING_BATCH_THRESHOLD = 1;
        bool LAYER_UNGROUPING = false;
        bool SINGLE_UNGROUPING = true;
        bool SUBTREE_UNGROUPING = false;
        bool IS_ACTION_INDEPENDENT = false;

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
            rhs.T_THRESHOLD                  = node["TThreshold"].as< double >(rhs.T_THRESHOLD);
            rhs.CONTINUE_PREVIOUS_SEARCH     = node["ContinuePreviousSearch"].as< bool >(rhs.CONTINUE_PREVIOUS_SEARCH);
            rhs.absBatch                     = node["AbstractionBatch"].as< int >(rhs.absBatch);
			rhs.batch_size                   = node["BatchSize"].as< int >(rhs.batch_size);
			rhs.random_abstraction           = node["RandomAbstraction"].as< bool >(rhs.random_abstraction);
            rhs.IS_PHI_UNGROUPING            = node["IsPhiUngrouping"].as< bool >(rhs.IS_PHI_UNGROUPING);
            rhs.UNGROUPING_BATCH_THRESHOLD   = node["UngroupingBatchThreshold"].as<double>(rhs.UNGROUPING_BATCH_THRESHOLD);
            rhs.IS_UNGROUPING                = node["IsUngrouping"].as<bool>(rhs.IS_UNGROUPING);
            rhs.IS_ACTION_INDEPENDENT        = node["IsActionIndependent"].as<bool>(rhs.IS_ACTION_INDEPENDENT);
            

			if (rhs.random_abstraction && (!rhs.DO_STATE_ABSTRACTION)) {
				std::cout<<"[Warning]: RandomAbstraction is set to true. However, DoStateAbstraction is set to false!"<<"\n";
			}
            return true;
        }
    };
}  // namespace YAML
