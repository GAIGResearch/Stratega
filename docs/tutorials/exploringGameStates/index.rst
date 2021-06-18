.. role:: cpp(code)
   :language: c++

#############################
Agents and Game States
#############################


.. note::
    This page is about **programming** for Stratega. You are in the right place if you want to know how to write code about querying aspects of the game state and information. 


AI Agents have access to the current game state in order to be able to determine what is the best next action to execute in the game. This page shows a few examples about how to
access interesting information in the game state that can help you implement agents for this framework. Stratega supplies information in two different modalities:

#. Static information: this is information about the game as it is defined. This information doesn't change from state to state - i.e. it's information about the game rules.
#. Dynamic information: includes all the elements of the game state, that changes from one instantiation of the game to another. Examples are the presence of units, buildings, player parameter values and technologies researched. 

Given that Stratega supports multiple games (defined via YAML files), the *static* information is useful for creating agents that can include domain knowledge for multiple games, or to train
agents that behave differently depending on the characteristics of the game being played. The *dynamic* information is, however, useful for decision making in a state to state basis and, with
the functionality of the forward model, permits the implementation of model-based and/or sampling agents.

This page shows examples of how agents can access this information. For a more lengthy description of the elements of the game state and its representation, see 
:ref:`Game State Representation <game-state-representation>`.


++++++++++++++++
Game Information
++++++++++++++++

.. note::
    Game Information refers to **static** information of a game being played in Stratega. You are in the right place if you want to know how to write
    code about querying aspects that are constant during the duration of the game (i.e. types of units, types of tiles, technologies defined...). 


First of all, the YAML description file of the game being played is accessible to the agents at all times. You can retrieve the filename and parse the YAML file yourself. For instance, 
like this:

.. code-block:: c++
    :linenos:

    std::string yamlPath = state.gameInfo->yamlPath;
    std::ifstream infile(yamlPath);
    std::string line;
    while (std::getline(infile, line))
    {
        // Each iteration is a 'line' in the YAML file.
    }


Note that 'state' in line 1 is the object of type 'GameState' which is (for instance) received in the function *computeAgent*. The property gameInfo in the game state is in charge of
providing static information about the game. See section :ref:`Game Info <game-state-representation_gameInfo>` for more details.


Game Description: Types
***********************

GameInfo's API provides information about the types of objects that can be found in the game. In particular, you can find information about possible types of tiles that form the board, 
entities and actions. These types can be accessed directly in their data structure (3 unordered maps, respectively, indexed by unique type ID) or by supplying a specific ID for the type.
The following code snippet shows how to access these maps and prints the name of the different elements in the game:

.. code-block:: c++
    :linenos:

    auto entityTypes = state.gameInfo->entityTypes;
    for (const auto& [id, eType] : *entityTypes) 
        std::cout << "[Entity] id: " << id << ", name: " << eType.name << std::endl;

    auto tileTypes = state.gameInfo->tileTypes;
    for (const auto& [id, tType] : *tileTypes) 
        std::cout << "[Tile] id: " <<  id << ", name: " << tType.name << std::endl;

    auto actionTypes = state.gameInfo->actionTypes;
    for (const auto& [id, aType] : *actionTypes) 
        std::cout << "[Action] id: " << id << ", name: " << aType.name << std::endl;


This code produces the following output for the game 'KillTheKing': 

.. code-block:: c++

    [Entity] id: 0, name: Archer
    [Entity] id: 1, name: Healer
    [Entity] id: 2, name: King
    [Entity] id: 3, name: Warrior
    [Tile] id: -1, name: FogOfWar
    [Tile] id: 0, name: Forest
    [Tile] id: 1, name: Mountain
    [Tile] id: 2, name: Plain
    [Tile] id: 3, name: Water
    [Action] id: 0, name: Attack
    [Action] id: 1, name: Heal
    [Action] id: 2, name: Move

All these *type* classes have information about the entities, tiles and actions they represent, such as:

#. `EntityType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/TileType.h>`_: Spawn/Build requirements, spawn costs, line of sights and actions that can be executed by this entity.
#. `TileType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/TileType.h>`_: Properties of the tile, such as if it's walkable or blocks the line of sight.
#. `ActionType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/ActionType.h>`_: Action source (player or entity), preconditions for the action to take place, list of effects and action targets.


You're encouraged to look at the :ref:`cpp Reference <cpp-reference>` for variables and methods that can be used to retrieve this information.


Game Description: Categories
****************************

The GameInfo object also provides extra (static) information about the game through a GameDescription pointer, which provides additional details on the game's ontology. Stratega defines
a collection of entity and action groups that allows the agent to query for entities or actions that respond to a particular purpose. For instance, in the game BasicTBS, included in the 
framework, the following groups are defined:

.. code-block:: yaml

    GameDescription:
        Type: CombatGame
        Actions:
            Move: [Move]
            Research: [Research]
            Gather: [Mine]
            Spawn: [Spawn, Build]
            Attack: [Attack]
        Entities:
            Base: [City]
            Building: [City, Barracks, MilitaryAcademy, Foundry, Workshop, Storage]
            Spawner: [City, Barracks, MilitaryAcademy]
            Unit: [Worker, Warrior, Archer, Catapult]
            Fighter: [Warrior, Archer, Catapult]
            NoFighter: [Worker]
            Melee: [Warrior]
            Ranged: [Archer, Catapult]

These descriptors can be retrieved using the *gameDescription* member of the GameInfo object. The following snippet prints the action categories for a player in the BasicTBS game.

.. code-block:: c++
    :linenos:

    for (const auto& [ac, actionTypes] : state.gameInfo->gameDescription->actionCategories)
    {
        std::string actionCategoryName = GameDescription::toString(ac);
        std::cout << "[Action Category] " << actionCategoryName << ": ";
        for (int actionTypeID : actionTypes)
        {
            auto actionType = state.gameInfo->getActionType(actionTypeID);
            std::cout << actionType.name << " (" << actionType.id << "), ";
        }
        std::cout << std::endl;
    }

Line 1 accesses the actionCategories map, which is indexed by a category and whose value is the set of action types that correspond to that category. Each category key is a value of 
the enumerator ActionCategory, in `GameDescription.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/GameDescription.h>`_, whose name can be
obtained by the function `toString` from the GameDescription class (line 3 above). Lines 5 to 9 show a loop through the vector of actions of this category, printing its name and its id.
Note that these IDs can be used to receive the action type object (from `ActionType.h <https://github.com/GAIGResearch/Stratega/blob/dev/Stratega/include/Stratega/Representation/ActionType.h>`_), 
as shown in line 7.

The output of the above snippet is as follows:

.. code-block:: text

    [Action Category] Attack: Attack (0),
    [Action Category] Gather: Mine (2),
    [Action Category] Move: Move (3),
    [Action Category] Research: Research (4),
    [Action Category] Spawn: Spawn (5), Build (1),


Similarly, it's possible to extract information about entity categories. The following snippet:

.. code-block:: c++
    :linenos:

    for (const auto& [ec, entityTypes] : state.gameInfo->gameDescription->entityCategories)
    {
        std::cout << "[Entity Category] " << GameDescription::toString(ec) << ": ";
        for (int entityTypeID : entityTypes)
        {
            auto entityType = state.gameInfo->getEntityType(entityTypeID);
            std::cout << entityType.name << " (" << entityType.id << "), ";
        }
        std::cout << std::endl;
    }

produces this output:

.. code-block:: text

    [Entity Category] Base: City (3),
    [Entity Category] Building: City (3), Barracks (1), MilitaryAcademy (6), Foundry (4), Workshop (10), Storage (7),
    [Entity Category] Fighter: Warrior (8), Archer (0), Catapult (2),
    [Entity Category] Melee: Warrior (8),
    [Entity Category] NoFighter: Worker (9),
    [Entity Category] Ranged: Archer (0), Catapult (2),
    [Entity Category] Spawner: City (3), Barracks (1), MilitaryAcademy (6),
    [Entity Category] Unit: Worker (9), Warrior (8), Archer (0), Catapult (2),


As can be seen, any of these types (entities or actions) can belong to more than one category.


Action types: conditions and effects
************************************

Action types also provide information about the conditions and effects that actions have in the game. These are all accessible
through the ActionType object, they are used as follows:

#. Action Preconditions: These are conditions for the actions to be applicable in its entirity. As soon as one of the preconditions of an action type is false, the game won't generate any of the action variants of this type. For instance, a Move action may indicate with a precondition that a minimum of movement points is required to move the unit; if this precondition is not fulfilled, no move actions will be generated.
#. Action Target Condition: These are conditions for actions to be applied to a particular target. These will only be checked if all preconditions (if any) are true. For instance, a move action may be valid or not depending on the destination of the movement.   
#. Effects: outcomes of the action, which are applied automatically on execution of the action.
#. OnStart effects: for continuous actions (those that take more than one turn/tick to execute), outcomes applied on its first turn/tick.
#. OnTick effects: for continuous actions, outcomes applied on every tick/turn.
#. OnComplete effects: for continuous actions, outcomes applied when the action finishes.
#. OnAbort effects: for continuous actions, effects applied when the action is aborted. 

As an example, the following code snippet accesses and prints the number of preconditions, conditions and effects of a given action type: 

.. code-block:: c++
    :linenos:

    //PRECONDITIONS
    auto preconditions = actionType.preconditions;
    if (preconditions.size() > 0) std::cout << " Preconditions: " << preconditions.size() << std::endl;
    for (const auto& precondition : preconditions)
        std::cout << "\t" << precondition->expr() << std::endl;
    

    //TARGET CONDITIONS
    if (actionType.actionTargets.size() > 0) std::cout << " Target Conditions: " << actionType.actionTargets.size() << std::endl;
    for (const auto& actionTarget : actionType.actionTargets)
    {
        for (auto condition : actionTarget.second)
            std::cout << "\t" << condition->expr() << std::endl;
    }

    //One-shot effects
    if (actionType.effects.size() > 0) std::cout << " Effects: " << actionType.effects.size() << std::endl;
    for (const auto& effect : actionType.effects)
        std::cout << "\t" << effect->expr() << std::endl;


When applied to the "KillTheKing" game, the output of executing this code is as follows:

.. code-block:: text

    [Attack]
     Target Conditions: 1
            InRange(Source, Target, Source.AttackRange)
     Effects: 1
            Attack(Target.Health, Source.AttackDamage)
    [Heal]
     Target Conditions: 1
            InRange(Source, Target, Source.HealRange)
     Effects: 1
            ModifyResource(Target.Health, Source.HealAmount)
    [Move]
     Preconditions: 1
            ResourceGreaterEqual(Source.MovementPoints, 1)
     Target Conditions: 1
            IsWalkable(Target)
     Effects: 2
            Move(Source, Target)
            ModifyResource(Source.MovementPoints, -1)


Note how in the action type "Move", there is a precondition that checks for a value of the parameter "MovementPoints" of at least 1 and, as target conditions, that the target tile is walkable.

The following example shows the output of the action type "Research" from the game "BasicTBS":

.. code-block:: text

    [Research]
     Target Conditions: 1
        CanResearch(Source, Target)
        CanAfford(Source, Target)
     OnStart Effects: 1
        PayCost(Source, Target)
     OnComplete Effects: 2
        Research(Source, Target)
        ModifyResource(Source.Player.Score, 10)


Technologies
************

Games in Stratega may have technologies that need to be researched in order to unlock certain 
abilities or entities. These technologies are organized in trees, and there may be more than one
tree per game. 

The GameInfo object exposes existing technologies in the game to the agent. For instance, it is 
possible to query the number of trees in the game and how many technologies each tree have, by
using the function 'getTechnologyCounts()':

.. code-block:: c++
    :linenos:

    std::unordered_map<int, int> techCounts = state.gameInfo->getTechnologyCounts();
    for (const auto& [id, count] : techCounts)
    {
        //'id' is the technology tree ID, 'count' is the number of technologies on each tree.
    }

Also, it is possible to retrieve the list of all technologies in a tree. The following code snippet
iterates through the technologies of all trees and prints the information to console:

.. code-block:: c++
    :linenos:

    std::unordered_map<int, int> techCounts = state.gameInfo->getTechnologyCounts();
    for (const auto& [id, count] : techCounts)
    {
        std::vector<TechnologyTreeNode> techs = state.gameInfo->getTreeNodes(id);
        for (TechnologyTreeNode t : techs)
            std::cout << t.toString(*state.gameInfo) << std::endl;
    }

This is part of the output obtained by this code for the BasicTBS game:

.. code-block:: text

    [...]
    Mining: Base technology (id: 7)
        Costs: Prod: 10.000000;
        Tech requirements (IDs): None.
        Research time: 2.000000

    Pottery: Allows to construct a Storage. (id: 8)
        Costs: Prod: 10.000000;
        Tech requirements (IDs): 7;
        Research time: 2.000000



++++++++++++++++
Game State
++++++++++++++++

.. note::
    Game Information refers to **dynamic** information of a game being played in Stratega. You are in the right place if you want to know how to write
    code about querying aspects that are *specific* to a given game state (actual positions of entities, values of properties, technologies researched, etc). 

