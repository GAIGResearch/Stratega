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
        Agent/UnitMCTSAgent/Transition.cpp
        Agent/UnitMCTSAgent/UnitMCTSAgent.cpp
        Agent/UnitMCTSAgent/UnitMCTSNode.cpp
        Agent/UnitMCTSAgent/UnitMCTSParameters.cpp
        Agent/MCTSAgent/MCTSAgent.cpp
        Agent/MCTSAgent/MCTSNode.cpp
        Agent/MCTSAgent/MCTSParameters.cpp
        Agent/OSLAAgent.cpp
        Agent/PortfolioForwardModel.cpp
        Agent/RandomAgent.cpp
        Agent/RHEAAgent/RHEAAgent.cpp
        Agent/RHEAAgent/RHEAGenome.cpp
        Agent/RHEAAgent/RHEAParameters.cpp
        Agent/RuleBasedAgents/KTKCombatAgent.cpp
        Agent/RuleBasedAgents/CombatAgent.cpp
        Agent/RuleBasedAgents/PusherAgent.cpp
        Agent/RuleBasedAgents/Direction.cpp
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
        GUI/GameRenderer.cpp
        Logging/FileLogger.cpp
        Logging/Log.cpp
        Logging/LoggingScope.cpp
        Representation/BuildContext.cpp
        Representation/Entity.cpp
        Representation/EntityType.cpp
        Representation/TileType.cpp
        Representation/GameDescription.cpp
        Representation/GameInfo.cpp
        Representation/GameState.cpp
        Representation/ActionQueue.cpp
        Representation/Player.cpp
        Representation/TechnologyTree.cpp
        Representation/Tile.cpp
        Utils/cparse/catch.cpp
        Utils/cparse/containers.cpp
        Utils/cparse/functions.cpp
        Utils/cparse/packToken.cpp
        Utils/cparse/shunting-yard.cpp
        Utils/cparse/builtin-features.cpp
        )
if(NOT SGA_BUILD_HEADLESS)
   list(APPEND STRATEGA_SOURCE_FILES
        GUI/AssetCache.cpp        
        GUI/TextureAtlas.cpp        
        NewGUI/GridLayout.cpp
        NewGUI/Widget.cpp
        NewGUI/World.cpp
        NewGUI/GridLayoutWidget.cpp
        NewGUI/MouseInformationWidget.cpp
        NewGUI/FOWControllerWidget.cpp
        NewGUI/PlayerInformationWidget.cpp
        NewGUI/GameStateInformationWidget.cpp
        NewGUI/ActionsWidget.cpp
        NewGUI/WorldControllerWidget.cpp
        NewGUI/SGARenderTarget.cpp
        NewGUI/SGADrawable.cpp

        NewGUI/GenericGameRenderer.cpp
        NewGUI/SpriteData.cpp
        NewGUI/ResourceManager.cpp
        )
message("Building GUI mode")
else()
message("Building HEADLESS mode")
endif()

#Copy Assets folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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

target_link_libraries_system(Stratega CONAN_PKG::boost)
target_link_libraries_system(Stratega CONAN_PKG::yaml-cpp)
target_link_libraries_system(Stratega CONAN_PKG::recastnavigation)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
target_link_libraries(Stratega Threads::Threads)
endif()

if(NOT SGA_BUILD_HEADLESS)
    target_link_libraries_system(Stratega CONAN_PKG::imgui)
    target_link_libraries_system(Stratega imgui)

    #if(UNIX AND NOT APPLE)
    if(SGA_BUILD_SFML_FROM_SOURCE OR CMAKE_SYSTEM_NAME MATCHES Linux)
        target_link_libraries_system(Stratega sfml-system)
        target_link_libraries_system(Stratega sfml-graphics)
        target_link_libraries_system(Stratega sfml-window)        
    else()
        target_link_libraries_system(Stratega CONAN_PKG::sfml)
    endif()
endif()

install(TARGETS
        Stratega
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include DESTINATION .)
