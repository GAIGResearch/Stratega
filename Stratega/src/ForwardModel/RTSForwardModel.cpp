#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <cstring>

namespace SGA
{
	void RTSForwardModel::advanceGameState(RTSGameState& state, const Action& action) const
	{
		if (!(action.actionTypeFlags==EndTickAction))
		{
			auto& actionType = state.getActionType(action.actionTypeID);
			if (actionType.sourceType == ActionSourceType::Unit)
			{
				auto& sourceUnit = action.targets[0].getEntity(state);
				/*if (sourceUnit == nullptr)
					return;*/

				sourceUnit.intendedAction = action;
			}
		
		}
		else // Advance game
		{
			// Update what the units are doing
			for (auto& unit : state.entities)
			{
				if (!unit.intendedAction.has_value())
				{
					unit.executingAction = unit.intendedAction;
					unit.intendedAction = {};
				}
			}

			for (auto& unit : state.entities)
			{
				if(unit.executingAction.has_value())
					executeAction(state, unit.executingAction.value());
			}
			
			resolveUnitCollisions(state);
			resolveEnvironmentCollisions(state);

			// Remove Entities
			for (size_t i = 0; i < state.entities.size(); i++)
			{
				if (state.entities[i].shouldRemove)
				{
					state.entities.erase(state.entities.begin() + i);
					i -= 1;
				}
			}

			// Update cooldown
			for (auto& unit : state.entities)
			{
				unit.actionCooldown = std::max(0., unit.actionCooldown - deltaTime);
			}

			endTick(state);
			state.isGameOver = checkGameIsFinished(state);
		}
	}

	std::vector<Action> RTSForwardModel::generateActions(RTSGameState& state) const
	{
		throw std::runtime_error("Can't generate actions without an playerID for RTS-Games");
	}

	std::vector<Action> RTSForwardModel::generateActions(RTSGameState& state, int playerID) const
	{
		return (EntityActionSpace().generateActions(state, playerID));
	}

	//void RTSForwardModel::executeMove(RTSGameState2& state, RTSUnit& unit) const
	//{
	//	if (!unit.executingAction.validate(state.target))
	//	{
	//		unit.executingAction.type = RTSActionType::None;
	//		return;
	//	}

	//	Vector2f targetPos = unit.executingAction.targetPosition;

	//	//Get end position of current path
	//	Vector2f oldTargetPos(0, 0);
	//	oldTargetPos.x = unit.path.m_straightPath[(unit.path.m_nstraightPath - 1) * 3];
	//	oldTargetPos.y = unit.path.m_straightPath[((unit.path.m_nstraightPath - 1) * 3) + 2];

	//	//Check if path is empty or is a diferent path to the target pos
	//	if (unit.path.m_nstraightPath == 0 || targetPos != oldTargetPos) {

	//		Path path = findPath(state.target, unit.position, targetPos);
	//		unit.path = path;
	//		unit.path.currentPathIndex++;
	//	}

	//	//Check if path has points to visit
	//	if (unit.path.m_nstraightPath > 0)
	//	{
	//		//Assign the current path index as target
	//		targetPos = Vector2f(unit.path.m_straightPath[unit.path.currentPathIndex * 3], unit.path.m_straightPath[unit.path.currentPathIndex * 3 + 2]);
	//	}

	//	auto movementDir = targetPos - unit.position;
	//	auto movementDistance = movementDir.magnitude();
	//	auto movementSpeed = unit.movementSpeed * deltaTime;
	//	if (movementDistance <= movementSpeed)
	//	{
	//		unit.path.currentPathIndex++;
	//		if (unit.path.m_nstraightPath <= unit.path.currentPathIndex)
	//		{
	//			if (movementDistance <= movementSpeed) {
	//				unit.position = targetPos;
	//				unit.executingAction.type = RTSActionType::None;
	//				unit.path = Path();
	//			}
	//		}
	//	}
	//	else
	//	{
	//		unit.position = unit.position + (movementDir / movementDir.magnitude()) * movementSpeed;
	//	}
	//}

	//void RTSForwardModel::executeAttack(RTSGameState2& state, RTSUnit& unit) const
	//{
	//	if (!unit.executingAction.validate(state.target))
	//	{
	//		if (unit.actionCooldown > 0)
	//		{
	//			unit.executingAction.type = RTSActionType::None;
	//		}

	//		unit.path = Path();
	//		return;
	//	}
	//	if (unit.actionCooldown > 0)
	//		return;

	//	auto* targetUnit = unit.state.get().getUnit(unit.executingAction.targetUnitID);

	//	//Update position of the action
	//	unit.executingAction.targetPosition = targetUnit->position;

	//	//If is in range attack, if not, move to to a closer position
	//	if (unit.position.distance(targetUnit->position) <= unit.actionRange)
	//	{
	//		targetUnit->health -= unit.attackDamage;
	//		if (targetUnit->health <= 0)
	//		{
	//			state.deadUnitIDs.emplace_back(targetUnit->unitID);
	//			unit.executingAction.type = RTSActionType::None;
	//			unit.path = Path();
	//		}
	//		unit.actionCooldown = unit.maxActionCooldown;
	//	}
	//	else
	//	{
	//		//Get end position of current path
	//		Vector2f oldTargetPos(0, 0);

	//		Vector2f targetPos = targetUnit->position;

	//		//Check if path is empty
	//		if (unit.path.m_nstraightPath > 0)
	//		{
	//			oldTargetPos.x = unit.path.m_straightPath[(unit.path.m_nstraightPath - 1) * 3];
	//			oldTargetPos.y = unit.path.m_straightPath[((unit.path.m_nstraightPath - 1) * 3) + 2];

	//			//Compare the path end with the target position
	//			if (oldTargetPos != targetPos)
	//			{
	//				//Update the path
	//				Path path = findPath(state.target, unit.position, targetPos);

	//				unit.path = path;
	//				unit.path.currentPathIndex++;
	//			}
	//		}
	//		else
	//		{
	//			//Update the new path
	//			Path path = findPath(state.target, unit.position, targetPos);

	//			unit.path = path;
	//			unit.path.currentPathIndex++;
	//		}

	//		//Check if path has points to visit

	//		if (unit.path.m_nstraightPath > 0)
	//		{
	//			//Assign the current path index as target
	//			targetPos = Vector2f(unit.path.m_straightPath[unit.path.currentPathIndex * 3], unit.path.m_straightPath[unit.path.currentPathIndex * 3 + 2]);

	//			auto movementDir = targetPos - unit.position;
	//			auto movementDistance = movementDir.magnitude();
	//			auto movementSpeed = unit.movementSpeed * deltaTime;
	//			if (movementDistance <= movementSpeed)
	//			{
	//				unit.path.currentPathIndex++;
	//				if (unit.path.m_nstraightPath <= unit.path.currentPathIndex)
	//				{
	//					if (movementDistance <= movementSpeed)
	//					{
	//						unit.position = targetPos;
	//						//unit.executingAction = nullptr;
	//						unit.path = Path();
	//					}
	//				}
	//			}
	//			else
	//			{
	//				unit.position = unit.position + (movementDir / movementDir.magnitude()) * movementSpeed;
	//			}
	//		}
	//	}
	//}

	//void RTSForwardModel::executeHeal(RTSGameState2& state, RTSUnit& unit) const
	//{
	//	if (!unit.executingAction.validate(state.target))
	//	{
	//		unit.executingAction.type = RTSActionType::None;
	//		return;
	//	}

	//	auto* targetUnit = unit.state.get().getUnit(unit.executingAction.targetUnitID);
	//	targetUnit->health += unit.healAmount;

	//	if (targetUnit->health > targetUnit->maxHealth)
	//		targetUnit->health = targetUnit->maxHealth;

	//	unit.actionCooldown = unit.maxActionCooldown;
	//}

	void RTSForwardModel::resolveUnitCollisions(RTSGameState& state) const
	{
		for (auto& unit : state.entities)
		{
			Vector2f pushDir;
			for (auto& otherUnit : state.entities)
			{
				// Units cant collide with themselves, also idle units do not push busy units
				if (unit.id == otherUnit.id /*|| (unit.executingAction.type != RTSActionType::None && otherUnit.executingAction.type == RTSActionType::None)*/)
					continue;

				const auto& entityType = state.getEntityType(unit.typeID);

				//Move action
				if (!entityType.canExecuteAction(2))
					continue;

				auto dir = otherUnit.position - unit.position;
				if (dir.magnitude() <= unit.collisionRadius + otherUnit.collisionRadius)
				{
					auto penetrationDepth = unit.collisionRadius + otherUnit.collisionRadius - dir.magnitude();
					pushDir = pushDir + dir.normalized() * 2 / (1 + penetrationDepth);
				}
			}

			unit.position = unit.position - pushDir * deltaTime;
		}
	}

	void RTSForwardModel::resolveEnvironmentCollisions(RTSGameState& state) const
	{
		static float RECT_SIZE = 1;

		// Collision
		for (auto& unit : state.entities)
		{
			int startCheckPositionX = std::floor(unit.position.x - unit.collisionRadius - RECT_SIZE);
			int endCheckPositionX = std::ceil(unit.position.x + unit.collisionRadius + RECT_SIZE);
			int startCheckPositionY = std::floor(unit.position.y - unit.collisionRadius - RECT_SIZE);
			int endCheckPositionY = std::ceil(unit.position.y + unit.collisionRadius + RECT_SIZE);

			Vector2f pushDir;
			for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
			{
				for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
				{
					// Everything outside bounds is considered as un-walkable tiles
					if (state.isInBounds({ x, y }) && state.board.get(x, y).isWalkable)
						continue;

					// https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
					auto fx = static_cast<float>(x);
					auto fy = static_cast<float>(y);
					auto nearestX = std::max(fx, std::min(unit.position.x, fx + RECT_SIZE));
					auto nearestY = std::max(fy, std::min(unit.position.y, fy + RECT_SIZE));
					auto dist = unit.position - Vector2f(nearestX, nearestY);

					auto penetrationDepth = unit.collisionRadius - dist.magnitude();
					// No collision detected
					if (penetrationDepth <= 0)
						continue;

					auto dir = dist.normalized() * penetrationDepth;
					pushDir = pushDir + dir;
				}
			}

			unit.position = unit.position + pushDir;
		}
	}

	bool RTSForwardModel::buildNavMesh(RTSGameState& state, NavigationConfig config) const
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		state.navigation = std::make_shared<Navigation>();

		state.navigation->config = config;

		//Get size from current board
		auto& board = state.board;
		float width = board.getWidth();
		float height = board.getHeight();
		float cellSize = 1;

		state.navigation->cleanup();

		float m_meshBMin[3]{ 0,0,0 };
		float m_meshBMax[3]{ width,10,height };
		const float* bmin = m_meshBMin;
		const float* bmax = m_meshBMax;

		//
		// Step 1. Initialize build config.
		//

		// Init build configuration from GUI
		memset(&state.navigation->m_cfg, 0, sizeof(state.navigation->m_cfg));

		state.navigation->m_cfg.cs = state.navigation->config.m_cellSize;
		state.navigation->m_cfg.ch = state.navigation->config.m_cellHeight;
		state.navigation->m_cfg.walkableSlopeAngle = state.navigation->config.m_agentMaxSlope;
		state.navigation->m_cfg.walkableHeight = (int)ceilf(state.navigation->config.m_agentHeight / state.navigation->m_cfg.ch);
		state.navigation->m_cfg.walkableClimb = (int)floorf(state.navigation->config.m_agentMaxClimb / state.navigation->m_cfg.ch);
		state.navigation->m_cfg.walkableRadius = (int)ceilf(state.navigation->config.m_agentRadius / state.navigation->m_cfg.cs);
		state.navigation->m_cfg.maxEdgeLen = (int)(state.navigation->config.m_edgeMaxLen / state.navigation->config.m_cellSize);
		state.navigation->m_cfg.maxSimplificationError = state.navigation->config.m_edgeMaxError;
		state.navigation->m_cfg.minRegionArea = (int)rcSqr(state.navigation->config.m_regionMinSize);		// Note: area = size*size
		state.navigation->m_cfg.mergeRegionArea = (int)rcSqr(state.navigation->config.m_regionMergeSize);	// Note: area = size*size
		state.navigation->m_cfg.maxVertsPerPoly = (int)state.navigation->config.m_vertsPerPoly;
		state.navigation->m_cfg.detailSampleDist = state.navigation->config.m_detailSampleDist < 0.9f ? 0 : state.navigation->config.m_cellSize * state.navigation->config.m_detailSampleDist;
		state.navigation->m_cfg.detailSampleMaxError = state.navigation->config.m_cellHeight * state.navigation->config.m_detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(state.navigation->m_cfg.bmin, bmin);
		rcVcopy(state.navigation->m_cfg.bmax, bmax);
		rcCalcGridSize(state.navigation->m_cfg.bmin, state.navigation->m_cfg.bmax, state.navigation->m_cfg.cs, &state.navigation->m_cfg.width, &state.navigation->m_cfg.height);

		//// Reset build times gathering.
		state.navigation->m_ctx.resetTimers();

		// Start the build process.	
		state.navigation->m_ctx.startTimer(RC_TIMER_TOTAL);

		//
		// Step 2. Rasterize input polygon soup.
		//

		// Allocate voxel heightfield where we rasterize our input data to.
		state.navigation->m_solid = rcAllocHeightfield();

		if (!state.navigation->m_solid)
		{
			std::cout << "buildNavigation: Out of memory 'solid'." << std::endl;
			return false;
		}

		if (!rcCreateHeightfield(&state.navigation->m_ctx, *state.navigation->m_solid, state.navigation->m_cfg.width, state.navigation->m_cfg.height, state.navigation->m_cfg.bmin, state.navigation->m_cfg.bmax, state.navigation->m_cfg.cs, state.navigation->m_cfg.ch))
		{
			std::cout << "buildNavigation: Could not create solid heightfield." << std::endl;
			return false;
		}

		// Where hf is a reference to an heightfield object.
		const float* orig = state.navigation->m_solid->bmin;
		const float cs = state.navigation->m_solid->cs;
		const float ch = state.navigation->m_solid->ch;
		const int w = state.navigation->m_solid->width;
		const int h = state.navigation->m_solid->height;


		for (float x = 0; x < w; x++)
		{
			for (float y = 0; y < h; y++)
			{
				int pos = (y * width + x);
				auto& tile = board.get(x, y);

				if (tile.isWalkable)
					rcAddSpan(&state.navigation->m_ctx, *state.navigation->m_solid, x, y, 0, 5, RC_WALKABLE_AREA, 0);
			}
		}

		//
		// Step 3. Filter walkables surfaces.
		//

		// Once all geoemtry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (state.navigation->config.m_filterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(&state.navigation->m_ctx, state.navigation->m_cfg.walkableClimb, *state.navigation->m_solid);
		if (state.navigation->config.m_filterLedgeSpans)
			rcFilterLedgeSpans(&state.navigation->m_ctx, state.navigation->m_cfg.walkableHeight, state.navigation->m_cfg.walkableClimb, *state.navigation->m_solid);
		if (state.navigation->config.m_filterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(&state.navigation->m_ctx, state.navigation->m_cfg.walkableHeight, *state.navigation->m_solid);

		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		state.navigation->m_chf = rcAllocCompactHeightfield();
		if (!state.navigation->m_chf)
		{
			std::cout << "buildNavigation: Out of memory 'chf'." << std::endl;
			return false;
		}
		if (!rcBuildCompactHeightfield(&state.navigation->m_ctx, state.navigation->m_cfg.walkableHeight, state.navigation->m_cfg.walkableClimb, *state.navigation->m_solid, *state.navigation->m_chf))
		{
			std::cout << "buildNavigation: Could not build compact data." << std::endl;
			return false;
		}

		// Erode the walkable area by agent radius.
		if (state.navigation->config.m_erodeWalkableArea)
			if (!rcErodeWalkableArea(&state.navigation->m_ctx, state.navigation->m_cfg.walkableRadius, *state.navigation->m_chf))
			{
				std::cout << "buildNavigation: Could not erode." << std::endl;
				return false;
			}


		// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
		// There are 3 martitioning methods, each with some pros and cons:
		// 1) Watershed partitioning
		//   - the classic Recast partitioning
		//   - creates the nicest tessellation
		//   - usually slowest
		//   - partitions the heightfield into nice regions without holes or overlaps
		//   - the are some corner cases where this method creates produces holes and overlaps
		//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
		//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
		//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
		// 2) Monotone partioning
		//   - fastest
		//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
		//   - creates long thin polygons, which sometimes causes paths with detours
		//   * use this if you want fast navmesh generation
		// 3) Layer partitoining
		//   - quite fast
		//   - partitions the heighfield into non-overlapping regions
		//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
		//   - produces better triangles than monotone partitioning
		//   - does not have the corner cases of watershed partitioning
		//   - can be slow and create a bit ugly tessellation (still better than monotone)
		//     if you have large open areas with small obstacles (not a problem if you use tiles)
		//   * good choice to use for tiled navmesh with medium and small sized tiles

		if (state.navigation->config.m_partitionType == SAMPLE_PARTITION_WATERSHED)
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(&state.navigation->m_ctx, *state.navigation->m_chf))
			{
				std::cout << "buildNavigation: Could not build distance field." << std::endl;
				return false;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(&state.navigation->m_ctx, *state.navigation->m_chf, 0, state.navigation->m_cfg.minRegionArea, state.navigation->m_cfg.mergeRegionArea))
			{
				std::cout << "buildNavigation: Could not build watershed regions." << std::endl;
				return false;
			}
		}
		else if (state.navigation->config.m_partitionType == SAMPLE_PARTITION_MONOTONE)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(&state.navigation->m_ctx, *state.navigation->m_chf, 0, state.navigation->m_cfg.minRegionArea, state.navigation->m_cfg.mergeRegionArea))
			{
				std::cout << "buildNavigation: Could not build monotone regions." << std::endl;
				return false;
			}
		}
		else // SAMPLE_PARTITION_LAYERS
		{
			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildLayerRegions(&state.navigation->m_ctx, *state.navigation->m_chf, 0, state.navigation->m_cfg.minRegionArea))
			{
				std::cout << "buildNavigation: Could not build layer regions." << std::endl;
				return false;
			}
		}

		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		state.navigation->m_cset = rcAllocContourSet();
		if (!state.navigation->m_cset)
		{
			std::cout << "buildNavigation: Out of memory 'cset'." << std::endl;
			return false;
		}
		if (!rcBuildContours(&state.navigation->m_ctx, *state.navigation->m_chf, state.navigation->m_cfg.maxSimplificationError, state.navigation->m_cfg.maxEdgeLen, *state.navigation->m_cset))
		{
			std::cout << "buildNavigation: Could not create contours." << std::endl;
			return false;
		}

		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		state.navigation->m_pmesh = rcAllocPolyMesh();
		if (!state.navigation->m_pmesh)
		{
			std::cout << "buildNavigation: Out of memory 'pmesh'." << std::endl;
			return false;
		}
		if (!rcBuildPolyMesh(&state.navigation->m_ctx, *state.navigation->m_cset, state.navigation->m_cfg.maxVertsPerPoly, *state.navigation->m_pmesh))
		{
			std::cout << "buildNavigation: Could not triangulate contours." << std::endl;
			return false;
		}

		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		state.navigation->m_dmesh = rcAllocPolyMeshDetail();
		if (!state.navigation->m_dmesh)
		{
			std::cout << "buildNavigation: Out of memory 'pmdtl'." << std::endl;
			return false;
		}

		if (!rcBuildPolyMeshDetail(&state.navigation->m_ctx, *state.navigation->m_pmesh, *state.navigation->m_chf, state.navigation->m_cfg.detailSampleDist, state.navigation->m_cfg.detailSampleMaxError, *state.navigation->m_dmesh))
		{
			std::cout << "buildNavigation: Could not build detail mesh." << std::endl;
			return false;
		}

		// At this point the navigation mesh data is ready

		//
		// Step 8. Create Detour data from Recast poly mesh.
		//

		// The GUI may allow more max points per polygon than Detour can handle.
		// Only build the detour navmesh if we do not exceed the limit.
		if (state.navigation->m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			unsigned char* navData = 0;
			int navDataSize = 0;

			// Update poly flags from areas.
			for (int i = 0; i < state.navigation->m_pmesh->npolys; ++i)
			{
				state.navigation->m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;
				state.navigation->m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = state.navigation->m_pmesh->verts;
			params.vertCount = state.navigation->m_pmesh->nverts;
			params.polys = state.navigation->m_pmesh->polys;
			params.polyAreas = state.navigation->m_pmesh->areas;
			params.polyFlags = state.navigation->m_pmesh->flags;
			params.polyCount = state.navigation->m_pmesh->npolys;
			params.nvp = state.navigation->m_pmesh->nvp;
			params.detailMeshes = state.navigation->m_dmesh->meshes;
			params.detailVerts = state.navigation->m_dmesh->verts;
			params.detailVertsCount = state.navigation->m_dmesh->nverts;
			params.detailTris = state.navigation->m_dmesh->tris;
			params.detailTriCount = state.navigation->m_dmesh->ntris;

			params.walkableHeight = state.navigation->config.m_agentHeight;
			params.walkableHeight = 2;
			params.walkableRadius = state.navigation->config.m_agentRadius;
			params.walkableClimb = state.navigation->config.m_agentMaxClimb;
			params.walkableClimb = 0.89f;
			rcVcopy(params.bmin, state.navigation->m_pmesh->bmin);
			rcVcopy(params.bmax, state.navigation->m_pmesh->bmax);
			params.cs = state.navigation->m_cfg.cs;
			params.ch = state.navigation->m_cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				std::cout << "Could not build Detour navmesh." << std::endl;
				return false;
			}

			state.navigation->m_navMesh = dtAllocNavMesh();
			if (!state.navigation->m_navMesh)
			{
				dtFree(navData);
				std::cout << "Could not create Detour navmesh" << std::endl;
				return false;
			}

			dtStatus status;

			status = state.navigation->m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status))
			{
				dtFree(navData);
				std::cout << "Could not init Detour navmesh" << std::endl;
				return false;
			}
			state.navigation->m_navQuery = dtAllocNavMeshQuery();

			status = state.navigation->m_navQuery->init(state.navigation->m_navMesh, 2048);
			if (dtStatusFailed(status))
			{
				std::cout << "Could not init Detour navmesh query" << std::endl;
				return false;
			}


		}

		if (state.navigation->m_navQuery)
		{
			// Change costs.
			state.navigation->m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
		}

		auto t2 = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		std::cout << "Build navmesh " << duration << std::endl;

		return true;
	}

	Path RTSForwardModel::findPath(const RTSGameState& state, Vector2f startPos, Vector2f endPos) const
	{
		//Convert grid pos to 3D pos
		float startPosV3[3];
		startPosV3[0] = startPos.x;
		startPosV3[1] = 0;
		startPosV3[2] = startPos.y;

		float endPosV3[3];
		endPosV3[0] = endPos.x;
		endPosV3[1] = 0;
		endPosV3[2] = endPos.y;

		//Start and end polys
		dtPolyRef startRef;
		dtPolyRef endRef;

		//Polys found in search
		dtPolyRef m_polys[MAX_POLYS];
		dtPolyRef m_parent[MAX_POLYS];
		int m_npolys;

		//Find nearest poly
		state.navigation->m_navQuery->findNearestPoly(startPosV3, state.navigation->m_polyPickExt, &state.navigation->m_filter, &startRef, startPosV3);
		state.navigation->m_navQuery->findNearestPoly(endPosV3, state.navigation->m_polyPickExt, &state.navigation->m_filter, &endRef, endPosV3);

		Path path;

		if (startRef && endRef)
		{
			state.navigation->m_navQuery->findPath(startRef, endRef, startPosV3, endPosV3, &state.navigation->m_filter, m_polys, &m_npolys, MAX_POLYS);
			path.m_nstraightPath = 0;
			if (m_npolys)
			{
				// In case of partial path, make sure the end point is clamped to the last polygon.			

				if (m_polys[m_npolys - 1] != endRef)
					state.navigation->m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], endPosV3, endPosV3, 0);

				state.navigation->m_navQuery->findStraightPath(startPosV3, endPosV3, m_polys, m_npolys,
					path.m_straightPath, path.m_straightPathFlags,
					path.m_straightPathPolys, &path.m_nstraightPath, MAX_POLYS, path.m_straightPathOptions);
			}
		}
		else
		{
			path.m_nstraightPath = 0;
		}

		return path;
	}

	bool RTSForwardModel::checkGameIsFinished(RTSGameState& state) const
	{
		/*int numberPlayerCanPlay = 0;
		int winnerID = -1;
		for (Player& player : state.players)
		{
			if (player.canPlay && canPlayerPlay(player))
			{
				winnerID = player.id;
				numberPlayerCanPlay++;
			}
			else
			{
				player.canPlay = false;
			}
		}

		if (numberPlayerCanPlay <= 1)
		{
			state.winnerPlayerID = (winnerID);
			return true;
		}*/

		return false;
	}

}
