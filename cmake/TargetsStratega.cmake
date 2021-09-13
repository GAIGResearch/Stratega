set(STRATEGA_DIR "${CMAKE_SOURCE_DIR}/stratega/")
set(STRATEGA_INCLUDE_DIR "${STRATEGA_DIR}/include")
set(STRATEGA_SRC_DIR "${STRATEGA_DIR}/src")

set(STRATEGA_SOURCE_FILES
        Agent/ActionScripts/AttackClosestOpponentScript.cpp
        Agent/ActionScripts/AttackWeakestOpponentScript.cpp
        Agent/ActionScripts/RandomActionScript.cpp
        Agent/ActionScripts/RunAwayFromOpponentsScript.cpp
        Agent/ActionScripts/RunToFriendlyUnitsScript.cpp
        Agent/ActionScripts/SkipTurnScript.cpp
        Agent/ActionScripts/UseSpecialAbilityScript.cpp
        Agent/Agent.cpp
        Agent/AgentFactory.cpp
        Agent/AgentParameters.cpp
        Agent/DoNothingAgent.cpp
        Agent/Heuristic/AbstractHeuristic.cpp
        Agent/Heuristic/MinimizeDistanceHeuristic.cpp
        Agent/MCTSAgent/MCTSAgent.cpp
        Agent/MCTSAgent/MCTSNode.cpp
        Agent/MCTSAgent/MCTSParameters.cpp
        Agent/OSLAAgent.cpp
        Agent/PortfolioForwardModel.cpp
        Agent/RandomAgent.cpp
        Agent/RHEAAgent/RHEAAgent.cpp
        Agent/RHEAAgent/RHEAGenome.cpp
        Agent/RHEAAgent/RHEAParameters.cpp
        Agent/RuleBasedAgents/CombatAgent.cpp
        Agent/ScriptedAgent.cpp
        Agent/StateAbstraction/StateFactory.cpp
        Agent/TreeSearchAgents/ActionAbstractionMCTSAgent.cpp
        Agent/TreeSearchAgents/BeamSearchAgent.cpp
        Agent/TreeSearchAgents/BFSAgent.cpp
        Agent/TreeSearchAgents/DFSAgent.cpp
        Agent/TreeSearchAgents/TreeNode.cpp
        Arena/Arena.cpp
        Configuration/FunctionParser.cpp
        Configuration/GameConfig.cpp
        Configuration/GameConfigParser.cpp
        ForwardModel/ActionAssignment.cpp
        ForwardModel/Action.cpp
        ForwardModel/ActionSpace.cpp
        ForwardModel/ActionTarget.cpp
        ForwardModel/Condition.cpp
        ForwardModel/Effect.cpp
        ForwardModel/ForwardModel.cpp
        ForwardModel/FunctionFactory.cpp
        ForwardModel/FunctionParameter.cpp
        ForwardModel/RTSForwardModel.cpp
        ForwardModel/SamplingMethod.cpp
        ForwardModel/TargetType.cpp
        ForwardModel/TBSForwardModel.cpp
        Game/AgentThread.cpp
        Game/GameRunner.cpp
        Game/RTSGameRunner.cpp
        Game/TBSGameRunner.cpp
        GUI/AssetCache.cpp
        GUI/EntityRenderer.cpp
        GUI/GameRenderer.cpp
        GUI/RTSGameRenderer.cpp
        GUI/TBSGameRenderer.cpp
        GUI/TileMap.cpp
        GUI/Widgets/ActionsController.cpp
        GUI/Widgets/FogOfWarController.cpp
        Logging/FileLogger.cpp
        Logging/Log.cpp
        Logging/LoggingScope.cpp
        Representation/BuildContext.cpp
        Representation/Entity.cpp
        Representation/EntityType.cpp
        Representation/GameDescription.cpp
        Representation/GameInfo.cpp
        Representation/GameState.cpp
        Representation/Player.cpp
        Representation/TechnologyTree.cpp
        Representation/Tile.cpp
        )

list(TRANSFORM STRATEGA_SOURCE_FILES PREPEND "${STRATEGA_SRC_DIR}/")

add_library(stratega STATIC ${STRATEGA_SOURCE_FILES})

target_include_directories(stratega PUBLIC ${STRATEGA_INCLUDE_DIR})
target_link_libraries(stratega
        PUBLIC
        CONAN_PKG::yaml-cpp
        #        CONAN_PKG::recastnavigation  # alternatively simply link against the entire pkg
        CONAN_LIB::recastnavigation_Recast
        CONAN_LIB::recastnavigation_Detour
        CONAN_LIB::recastnavigation_DebugUtils
        )

#add_library(CONAN_PKG::smfl ALIAS sfml)

target_link_libraries(stratega
        INTERFACE
        project_options
        project_warnings
        PRIVATE
        "$<$<PLATFORM_ID:Linux>:sfml>"  # for linux we use target `sfml` directly
        "$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>"  # other platforms use Conan's `sfml`
        CONAN_PKG::imgui)

if (TARGET Threads::Threads)
    target_link_libraries(stratega PUBLIC Threads::Threads)
endif ()

install(TARGETS
        stratega
        #       yaml-cpp
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

get_target_property(stratega_cxx stratega CXX_STANDARD)
get_target_property(proj_options_cxx project_options CXX_STANDARD)
message("Stratega standard is: ${stratega_cxx}")
message("PROJ standard is: ${proj_options_cxx}")

install(DIRECTORY include DESTINATION .)
get_target_property(YAML_CPP_INCLUDE_DIRECTORIES CONAN_PKG::yaml-cpp INCLUDE_DIRECTORIES)
install(DIRECTORY ${YAML_CPP_INCLUDE_DIRECTORIES} DESTINATION .)


target_compile_options(stratega PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        #        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
        )
string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")