#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <cstring>
#pragma warning(disable: 5045)
namespace SGA
{
	void RTSForwardModel::advanceGameState(GameState& state, const Action& action) const
	{
		ActionAssignment assignmentCache;
		assignmentCache.clear();
		assignmentCache.assignActionOrReplace(action);

		advanceGameState(state, assignmentCache);
	}

	void RTSForwardModel::advanceGameState(GameState& state, const ActionAssignment& actions) const
	{
		moveEntities(state);
		resolveEntityCollisions(state);
		resolveEnvironmentCollisions(state);

		for (const auto& action : actions.getPlayerActions())
		{
			executeAction(state, action.second);
		}
		
		for(const auto& action : actions.getEntityActions())
		{
			executeAction(state, action.second);
		}

		executeOnAdvanceEffects(state);

		//Remove flagged entities
		auto& entities = state.getEntities();
		auto it = entities.begin();
		while (it != entities.end())
		{
			if (it->flagged())  it = entities.erase(it);
			else 				it++;
		}

		moveEntities(state);
		resolveEntityCollisions(state);
		resolveEnvironmentCollisions(state);

		endTick(state);
		state.setGameOver(checkGameIsFinished(state));
	}

	void RTSForwardModel::moveEntities(GameState& state) const
	{
		for(auto& entity : state.getEntities())
		{
			if (entity.getPath().isEmpty())
				continue;
			
			// Move to the next checkpoint in the path
			int cPathIdx = entity.getPath().currentPathIndex;
			auto targetPos = Vector2f(entity.getPath().m_straightPath[cPathIdx * 3], entity.getPath().m_straightPath[cPathIdx * 3 + 2]);

			auto movementDir = targetPos - entity.getPosition();
			auto movementDistance = movementDir.magnitude();
			auto movementSpeed = entity.getMovementSpeed() * deltaTime;
			if (movementDistance <= movementSpeed)
			{
				entity.incPathIndex();
				// Did we reach the end of the path?
				if (entity.getPath().m_nstraightPath <= entity.getPath().currentPathIndex)
				{
						entity.setPosition(targetPos);
						entity.setPath(Path());
				}
			}
			else
			{
				entity.setPosition(entity.getPosition() + (movementDir / movementDir.magnitude()) * movementSpeed);
			}
		}
	}

	void RTSForwardModel::resolveEntityCollisions(GameState& state) const
	{
		for (auto& unit : state.getEntities())
		{
			Vector2f pushDir;
			for (auto& otherUnit : state.getEntities())
			{
				// Units cant collide with themselves, also idle units do not push busy units
				if (unit.getID() == otherUnit.getID() /*|| (unit.executingAction.type != RTSActionType::None && otherUnit.executingAction.type == RTSActionType::None)*/)
					continue;

				const auto& entityType = unit.getEntityType();

				//Only affects enviroment collision if the entity can move
				int moveActionID = state.getGameInfo()->getActionTypeID("Move");
				if (!entityType.canExecuteAction(moveActionID))
					continue;

				auto dir = otherUnit.getPosition() - unit.getPosition();
				if (dir.magnitude() <= unit.getCollisionRadius() + otherUnit.getCollisionRadius())
				{
					auto penetrationDepth = unit.getCollisionRadius() + otherUnit.getCollisionRadius() - dir.magnitude();
					pushDir = pushDir + dir.normalized() * 2 / (1 + penetrationDepth);
					
					//Soft collision
					pushDir =pushDir/4;
				}
			}

			unit.setPosition(unit.getPosition() - pushDir * deltaTime);
		}
	}

	void RTSForwardModel::resolveEnvironmentCollisions(GameState& state) const
	{
		static float RECT_SIZE = 1;

		// Collision
		for (auto& unit : state.getEntities())
		{
			int startCheckPositionX = static_cast<int>(std::floor(unit.x() - unit.getCollisionRadius() - static_cast<double>(RECT_SIZE)));
			int endCheckPositionX = static_cast<int>(std::ceil(unit.x() + unit.getCollisionRadius() + static_cast<double>(RECT_SIZE)));
			int startCheckPositionY = static_cast<int>(std::floor(unit.y() - unit.getCollisionRadius() - static_cast<double>(RECT_SIZE)));
			int endCheckPositionY = static_cast<int>(std::ceil(unit.y() + unit.getCollisionRadius() + static_cast<double>(RECT_SIZE)));

			const auto& entityType = unit.getEntityType();
			
			//Only affects enviroment collision if the entity can move
			int moveActionID = state.getGameInfo()->getActionTypeID("Move");
			if (!entityType.canExecuteAction(moveActionID))
				continue;
			
			Vector2f pushDir;
			for (int x = startCheckPositionX; x <= endCheckPositionX; x++)
			{
				for (int y = startCheckPositionY; y <= endCheckPositionY; y++)
				{
					// Everything outside bounds is considered as un-walkable tiles
					if (state.isInBounds(Vector2i{ x, y }) && state.getTileAt({ x, y }).isWalkable())
						continue;

					// https://stackoverflow.com/questions/45370692/circle-rectangle-collision-response
					auto fx = static_cast<double>(x);
					auto fy = static_cast<double>(y);
					auto nearestX = std::max(fx, std::min(unit.x(), fx + static_cast<double>(RECT_SIZE)));
					auto nearestY = std::max(fy, std::min(unit.y(), fy + static_cast<double>(RECT_SIZE)));
					auto dist = unit.getPosition() - Vector2f(nearestX, nearestY);

					auto penetrationDepth = unit.getCollisionRadius() - dist.magnitude();
					// No collision detected
					if (penetrationDepth <= 0)
						continue;

					auto dir = dist.normalized() * penetrationDepth;
					pushDir = pushDir + dir;
					
				}
			}

			unit.setPosition(unit.getPosition() + pushDir);
		}
	}

	bool RTSForwardModel::buildNavMesh(GameState& state, NavigationConfig config) const
	{
		auto t1 = std::chrono::high_resolution_clock::now();

		std::shared_ptr<Navigation> nav = std::make_shared<Navigation>();
		state.setRTSNavigation(nav);

		nav->config = config;

		//Get size from current board
		float width = static_cast<float>(state.getBoardWidth());
		float height = static_cast<float>(state.getBoardHeight());

		state.getRTSNavigation()->cleanup();

		float m_meshBMin[3]{ 0,0,0 };
		float m_meshBMax[3]{ width,10,height };
		const float* bmin = m_meshBMin;
		const float* bmax = m_meshBMax;

		//
		// Step 1. Initialize build config.
		//

		// Init build configuration from GUI
		memset(&state.getRTSNavigation()->m_cfg, 0, sizeof(state.getRTSNavigation()->m_cfg));



		nav->m_cfg.cs = nav->config.m_cellSize;
		nav->m_cfg.ch = nav->config.m_cellHeight;
		nav->m_cfg.walkableSlopeAngle = nav->config.m_agentMaxSlope;
		nav->m_cfg.walkableHeight = static_cast<int>(ceilf(nav->config.m_agentHeight / nav->m_cfg.ch));
		nav->m_cfg.walkableClimb = static_cast<int>(floorf(nav->config.m_agentMaxClimb / nav->m_cfg.ch));
		nav->m_cfg.walkableRadius = static_cast<int>(ceilf(nav->config.m_agentRadius / nav->m_cfg.cs));
		nav->m_cfg.maxEdgeLen = static_cast<int>(nav->config.m_edgeMaxLen / nav->config.m_cellSize);
		nav->m_cfg.maxSimplificationError = nav->config.m_edgeMaxError;
		nav->m_cfg.minRegionArea = static_cast<int>(rcSqr(nav->config.m_regionMinSize));		// Note: area = size*size
		nav->m_cfg.mergeRegionArea = static_cast<int>(rcSqr(nav->config.m_regionMergeSize));	// Note: area = size*size
		nav->m_cfg.maxVertsPerPoly = static_cast<int>(nav->config.m_vertsPerPoly);
		nav->m_cfg.detailSampleDist = nav->config.m_detailSampleDist < 0.9f ? 0 : nav->config.m_cellSize * nav->config.m_detailSampleDist;
		nav->m_cfg.detailSampleMaxError = nav->config.m_cellHeight * nav->config.m_detailSampleMaxError;

		// Set the area where the navigation will be build.
		// Here the bounds of the input mesh are used, but the
		// area could be specified by an user defined box, etc.
		rcVcopy(nav->m_cfg.bmin, bmin);
		rcVcopy(nav->m_cfg.bmax, bmax);
		rcCalcGridSize(nav->m_cfg.bmin, nav->m_cfg.bmax, nav->m_cfg.cs, &nav->m_cfg.width, &nav->m_cfg.height);

		//// Reset build times gathering.
		nav->m_ctx.resetTimers();

		// Start the build process.	
		nav->m_ctx.startTimer(RC_TIMER_TOTAL);

		//
		// Step 2. Rasterize input polygon soup.
		//

		// Allocate voxel heightfield where we rasterize our input data to.
		nav->m_solid = rcAllocHeightfield();

		if (!nav->m_solid)
		{
			std::cout << "buildNavigation: Out of memory 'solid'." << std::endl;
			return false;
		}

		if (!rcCreateHeightfield(&nav->m_ctx, *nav->m_solid, nav->m_cfg.width, nav->m_cfg.height, nav->m_cfg.bmin, nav->m_cfg.bmax, nav->m_cfg.cs, nav->m_cfg.ch))
		{
			std::cout << "buildNavigation: Could not create solid heightfield." << std::endl;
			return false;
		}

		// Where hf is a reference to an heightfield object.
		// const float* orig = nav->m_solid->bmin;
		// const float cs = nav->m_solid->cs;
		// const float ch = nav->m_solid->ch;
		const int w = nav->m_solid->width;
		const int h = nav->m_solid->height;


		for (auto x = 0; x < w; x++)
		{
			for (auto y = 0; y < h; y++)
			{
				auto& tile = state.getTileAt({ x, y });
				if (tile.isWalkable())
					rcAddSpan(&nav->m_ctx, *nav->m_solid, x, y, 0, 5, RC_WALKABLE_AREA, 0);
			}
		}

		//
		// Step 3. Filter walkables surfaces.
		//

		// Once all geoemtry is rasterized, we do initial pass of filtering to
		// remove unwanted overhangs caused by the conservative rasterization
		// as well as filter spans where the character cannot possibly stand.
		if (nav->config.m_filterLowHangingObstacles)
			rcFilterLowHangingWalkableObstacles(&nav->m_ctx, nav->m_cfg.walkableClimb, *nav->m_solid);
		if (nav->config.m_filterLedgeSpans)
			rcFilterLedgeSpans(&nav->m_ctx, nav->m_cfg.walkableHeight, nav->m_cfg.walkableClimb, *nav->m_solid);
		if (nav->config.m_filterWalkableLowHeightSpans)
			rcFilterWalkableLowHeightSpans(&nav->m_ctx, nav->m_cfg.walkableHeight, *nav->m_solid);

		//
		// Step 4. Partition walkable surface to simple regions.
		//

		// Compact the heightfield so that it is faster to handle from now on.
		// This will result more cache coherent data as well as the neighbours
		// between walkable cells will be calculated.
		nav->m_chf = rcAllocCompactHeightfield();
		if (!nav->m_chf)
		{
			std::cout << "buildNavigation: Out of memory 'chf'." << std::endl;
			return false;
		}
		if (!rcBuildCompactHeightfield(&nav->m_ctx, nav->m_cfg.walkableHeight, nav->m_cfg.walkableClimb, *nav->m_solid, *nav->m_chf))
		{
			std::cout << "buildNavigation: Could not build compact data." << std::endl;
			return false;
		}

		// Erode the walkable area by agent radius.
		if (nav->config.m_erodeWalkableArea)
			if (!rcErodeWalkableArea(&nav->m_ctx, nav->m_cfg.walkableRadius, *nav->m_chf))
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

		if (nav->config.m_partitionType == SAMPLE_PARTITION_WATERSHED)
		{
			// Prepare for region partitioning, by calculating distance field along the walkable surface.
			if (!rcBuildDistanceField(&nav->m_ctx, *nav->m_chf))
			{
				std::cout << "buildNavigation: Could not build distance field." << std::endl;
				return false;
			}

			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildRegions(&nav->m_ctx, *nav->m_chf, 0, nav->m_cfg.minRegionArea, nav->m_cfg.mergeRegionArea))
			{
				std::cout << "buildNavigation: Could not build watershed regions." << std::endl;
				return false;
			}
		}
		else if (nav->config.m_partitionType == SAMPLE_PARTITION_MONOTONE)
		{
			// Partition the walkable surface into simple regions without holes.
			// Monotone partitioning does not need distancefield.
			if (!rcBuildRegionsMonotone(&nav->m_ctx, *nav->m_chf, 0, nav->m_cfg.minRegionArea, nav->m_cfg.mergeRegionArea))
			{
				std::cout << "buildNavigation: Could not build monotone regions." << std::endl;
				return false;
			}
		}
		else // SAMPLE_PARTITION_LAYERS
		{
			// Partition the walkable surface into simple regions without holes.
			if (!rcBuildLayerRegions(&nav->m_ctx, *nav->m_chf, 0, nav->m_cfg.minRegionArea))
			{
				std::cout << "buildNavigation: Could not build layer regions." << std::endl;
				return false;
			}
		}

		//
		// Step 5. Trace and simplify region contours.
		//

		// Create contours.
		nav->m_cset = rcAllocContourSet();
		if (!nav->m_cset)
		{
			std::cout << "buildNavigation: Out of memory 'cset'." << std::endl;
			return false;
		}
		if (!rcBuildContours(&nav->m_ctx, *nav->m_chf, nav->m_cfg.maxSimplificationError, nav->m_cfg.maxEdgeLen, *nav->m_cset))
		{
			std::cout << "buildNavigation: Could not create contours." << std::endl;
			return false;
		}

		//
		// Step 6. Build polygons mesh from contours.
		//

		// Build polygon navmesh from the contours.
		nav->m_pmesh = rcAllocPolyMesh();
		if (!nav->m_pmesh)
		{
			std::cout << "buildNavigation: Out of memory 'pmesh'." << std::endl;
			return false;
		}
		if (!rcBuildPolyMesh(&nav->m_ctx, *nav->m_cset, nav->m_cfg.maxVertsPerPoly, *nav->m_pmesh))
		{
			std::cout << "buildNavigation: Could not triangulate contours." << std::endl;
			return false;
		}

		//
		// Step 7. Create detail mesh which allows to access approximate height on each polygon.
		//

		nav->m_dmesh = rcAllocPolyMeshDetail();
		if (!nav->m_dmesh)
		{
			std::cout << "buildNavigation: Out of memory 'pmdtl'." << std::endl;
			return false;
		}

		if (!rcBuildPolyMeshDetail(&nav->m_ctx, *nav->m_pmesh, *nav->m_chf, nav->m_cfg.detailSampleDist, nav->m_cfg.detailSampleMaxError, *nav->m_dmesh))
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
		if (nav->m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
		{
			unsigned char* navData = 0;
			int navDataSize = 0;

			// Update poly flags from areas.
			for (int i = 0; i < nav->m_pmesh->npolys; ++i)
			{
				nav->m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;
				nav->m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			params.verts = nav->m_pmesh->verts;
			params.vertCount = nav->m_pmesh->nverts;
			params.polys = nav->m_pmesh->polys;
			params.polyAreas = nav->m_pmesh->areas;
			params.polyFlags = nav->m_pmesh->flags;
			params.polyCount = nav->m_pmesh->npolys;
			params.nvp = nav->m_pmesh->nvp;
			params.detailMeshes = nav->m_dmesh->meshes;
			params.detailVerts = nav->m_dmesh->verts;
			params.detailVertsCount = nav->m_dmesh->nverts;
			params.detailTris = nav->m_dmesh->tris;
			params.detailTriCount = nav->m_dmesh->ntris;

			params.walkableHeight = nav->config.m_agentHeight;
			params.walkableHeight = 2;
			params.walkableRadius = nav->config.m_agentRadius;
			params.walkableClimb = nav->config.m_agentMaxClimb;
			params.walkableClimb = 0.89f;
			rcVcopy(params.bmin, nav->m_pmesh->bmin);
			rcVcopy(params.bmax, nav->m_pmesh->bmax);
			params.cs = nav->m_cfg.cs;
			params.ch = nav->m_cfg.ch;
			params.buildBvTree = true;

			if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
			{
				std::cout << "Could not build Detour navmesh." << std::endl;
				return false;
			}

			nav->m_navMesh = dtAllocNavMesh();
			if (!nav->m_navMesh)
			{
				dtFree(navData);
				std::cout << "Could not create Detour navmesh" << std::endl;
				return false;
			}

			dtStatus status;

			status = nav->m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
			if (dtStatusFailed(status))
			{
				dtFree(navData);
				std::cout << "Could not init Detour navmesh" << std::endl;
				return false;
			}
			nav->m_navQuery = dtAllocNavMeshQuery();

			status = nav->m_navQuery->init(nav->m_navMesh, 2048);
			if (dtStatusFailed(status))
			{
				std::cout << "Could not init Detour navmesh query" << std::endl;
				return false;
			}


		}

		if (nav->m_navQuery)
		{
			// Change costs.
			nav->m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
		}

		auto t2 = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		std::cout << "Build navmesh " << duration << std::endl;

		return true;
	}

	Path RTSForwardModel::findPath(const GameState& state, Vector2f startPos, Vector2f endPos) const
	{
		//Convert grid pos to 3D pos
		float startPosV3[3];
		startPosV3[0] = static_cast<float>(startPos.x);
		startPosV3[1] = 0;
		startPosV3[2] = static_cast<float>(startPos.y);

		float endPosV3[3];
		endPosV3[0] = static_cast<float>(endPos.x);
		endPosV3[1] = 0;
		endPosV3[2] = static_cast<float>(endPos.y);

		//Start and end polys
		dtPolyRef startRef;
		dtPolyRef endRef;

		//Polys found in search
		dtPolyRef m_polys[MAX_POLYS];
		int m_npolys;

		//Find nearest poly
		std::shared_ptr<Navigation> nav = state.getRTSNavigation();
		nav->m_navQuery->findNearestPoly(startPosV3, nav->m_polyPickExt, &nav->m_filter, &startRef, startPosV3);
		nav->m_navQuery->findNearestPoly(endPosV3, nav->m_polyPickExt, &nav->m_filter, &endRef, endPosV3);

		Path path;

		if (startRef && endRef)
		{
			nav->m_navQuery->findPath(startRef, endRef, startPosV3, endPosV3, &nav->m_filter, m_polys, &m_npolys, MAX_POLYS);
			path.m_nstraightPath = 0;
			if (m_npolys)
			{
				// In case of partial path, make sure the end point is clamped to the last polygon.			

				if (m_polys[m_npolys - 1] != endRef)
					nav->m_navQuery->closestPointOnPoly(m_polys[m_npolys - 1], endPosV3, endPosV3, 0);

				nav->m_navQuery->findStraightPath(startPosV3, endPosV3, m_polys, m_npolys,
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

	bool RTSForwardModel::checkGameIsFinished(GameState& state) const
	{
		int numberPlayerCanPlay = 0;
		int winnerID = -1;
		for (Player& player : state.getPlayers())
		{
			//Check if player won
			if(player.canPlay() && checkPlayerWon(state, player.getID()))
			{
				winnerID = player.getID();
				
				state.setWinnerID(winnerID);
				return true;
			}

			//Check if player can play
			if (player.canPlay() && !checkPlayerLost(state, player.getID()))
			{
				winnerID = player.getID();
				numberPlayerCanPlay++;
			}
			else
			{
				player.setCanPlay(false);
			}
		}

		if (numberPlayerCanPlay <= 1)
		{
			state.setWinnerID(winnerID);
			return true;
		}

		return false;
	}

	std::unique_ptr<ForwardModel> RTSForwardModel::clone() const
	{
		return std::make_unique<RTSForwardModel>(*this);
	}
}