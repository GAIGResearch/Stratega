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

list(TRANSFORM STRATEGA_SOURCE_FILES PREPEND "${SUBPROJ_STRATEGA_SRC_DIR}/")

add_library(Stratega STATIC ${STRATEGA_SOURCE_FILES})

target_include_directories(Stratega PUBLIC ${SUBPROJ_STRATEGA_INCLUDE_DIR})

function(target_link_libraries_system target)
  set(libs ${ARGN})
  foreach(lib ${libs})
    get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
    target_link_libraries(${target} ${lib} project_options
        project_warnings)
  endforeach(lib)
endfunction(target_link_libraries_system)

target_link_libraries_system(Stratega CONAN_PKG::yaml-cpp)
target_link_libraries_system(Stratega CONAN_PKG::recastnavigation)
target_link_libraries_system(Stratega CONAN_PKG::imgui)
target_link_libraries_system(Stratega imgui)

if(UNIX AND NOT APPLE)
    target_link_libraries_system(Stratega sfml-system)
    target_link_libraries_system(Stratega sfml-graphics)
    target_link_libraries_system(Stratega sfml-window)
else()
    target_link_libraries_system(Stratega CONAN_PKG::sfml)
endif()

install(TARGETS
        Stratega
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include DESTINATION .)
