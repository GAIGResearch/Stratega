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
#target_compile_options(Stratega PRIVATE project_options
#        project_warnings)

#function(target_link_libraries_system target)
#  set(libs ${ARGN})
#  foreach(lib ${libs})
#    get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
#    target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
#    target_link_libraries(${target} ${lib}  INTERFACE_SYSTEM_INCLUDE_DIRECTORIES project_options
#        project_warnings)
#        message("${lib_include_dirs}")
#  endforeach(lib)
#endfunction(target_link_libraries_system)
#
#target_link_libraries_system(Stratega CONAN_PKG::yaml-cpp)
#target_link_libraries_system(Stratega CONAN_PKG::recastnavigation)
#target_link_libraries_system(Stratega $<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>)
#target_link_libraries_system(Stratega $<$<PLATFORM_ID:Linux>:sfml-system>)
#target_link_libraries_system(Stratega $<$<PLATFORM_ID:Linux>:sfml-graphics>)
#target_link_libraries_system(Stratega $<$<PLATFORM_ID:Linux>:sfml-window>)

target_link_libraries(Stratega
        PUBLIC
        #CONAN_PKG::opengl
        project_options
        project_warnings
        # TODO: Is this public private separation of modules accurate?
        CONAN_PKG::yaml-cpp
        CONAN_PKG::recastnavigation
        "$<$<TARGET_EXISTS:Threads::Threads>:Threads::Threads>"  #use threads if the target exists
        PRIVATE        
        CONAN_PKG::imgui
        imgui
        # other platforms use Conan's `sfml`
        "$<$<NOT:$<PLATFORM_ID:Linux>>:CONAN_PKG::sfml>"
        # for linux we have to use the targets of `sfml`'s components individually
        "$<$<PLATFORM_ID:Linux>:sfml-system>"
        "$<$<PLATFORM_ID:Linux>:sfml-graphics>"
        "$<$<PLATFORM_ID:Linux>:sfml-window>"
        )

install(TARGETS
        Stratega
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include DESTINATION .)
