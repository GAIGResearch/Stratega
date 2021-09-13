#pragma once
#include <Stratega/Representation/BuildContext.h>
#include "DetourCommon.h"
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourDebugDraw.h"

/// These are just sample areas
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND
};

enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

static const int MAX_POLYS = 256;

namespace SGA
{
	class NavigationConfig
	{
	public:
		NavigationConfig()
		{
			m_cellSize = 1;
			m_cellHeight = 0.2f;
			m_agentHeight = 2.0f;
			m_agentRadius = 0.6f;
			m_agentMaxClimb = 0.9f;
			m_agentMaxSlope = 45.0f;
			m_regionMinSize = 8;
			m_regionMergeSize = 20;
			m_edgeMaxLen = 0.5f;
			m_edgeMaxError = 0.1f;
			m_vertsPerPoly = 3.0f;
			m_detailSampleDist = 6.0f;
			m_detailSampleMaxError = 1.0f;
			m_partitionType = SAMPLE_PARTITION_WATERSHED;

			m_erodeWalkableArea = true;
			m_filterLowHangingObstacles = false;
			m_filterLedgeSpans = false;
			m_filterWalkableLowHeightSpans = false;
		}

		//Configuration
		float m_cellSize;
		float m_cellHeight;
		float m_agentHeight;
		float m_agentRadius;
		float m_agentMaxClimb;
		float m_agentMaxSlope;
		float m_regionMinSize;
		float m_regionMergeSize;
		float m_edgeMaxLen;
		float m_edgeMaxError;
		float m_vertsPerPoly;
		float m_detailSampleDist;
		float m_detailSampleMaxError;
		int m_partitionType;

		//Filter stuff
		bool m_filterLowHangingObstacles;
		bool m_filterLedgeSpans;
		bool m_filterWalkableLowHeightSpans;

		//Erode
		bool m_erodeWalkableArea;

		void resetSettings()
		{
			m_cellSize = 1;
			m_cellHeight = 0.2f;
			m_agentHeight = 2.0f;
			m_agentRadius = 0.6f;
			m_agentMaxClimb = 0.9f;
			m_agentMaxSlope = 45.0f;
			m_regionMinSize = 8;
			m_regionMergeSize = 20;
			m_edgeMaxLen = 0.5f;
			m_edgeMaxError = 0.1f;
			m_vertsPerPoly = 3.0f;
			m_detailSampleDist = 6.0f;
			m_detailSampleMaxError = 1.0f;
			m_partitionType = SAMPLE_PARTITION_WATERSHED;
			m_erodeWalkableArea = true;
			m_filterLowHangingObstacles = false;
			m_filterLedgeSpans = false;
			m_filterWalkableLowHeightSpans = false;
		}
	};
	
	class Navigation
	{
	public:
		Navigation() :
			m_solid(0),
			m_chf(0),
			m_cset(0),
			m_pmesh(0),
			m_cfg(),
			m_dmesh(0),
			m_navMesh(0),
			m_navQuery(0)
		{
			m_filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
			m_filter.setExcludeFlags(0);

			//Search radius 
			m_polyPickExt[0] = 2;
			m_polyPickExt[1] = 2;
			m_polyPickExt[2] = 2;

			config.resetSettings();
		}

		BuildContext m_ctx;

		//Recast Stuff
		rcHeightfield* m_solid;
		rcCompactHeightfield* m_chf;
		rcContourSet* m_cset;
		rcPolyMesh* m_pmesh;
		rcConfig m_cfg;
		rcPolyMeshDetail* m_dmesh;

		dtNavMesh* m_navMesh;
		dtNavMeshQuery* m_navQuery;

		NavigationConfig config;
		void cleanup()
		{
			rcFreeHeightField(m_solid);
			m_solid = 0;
			rcFreeCompactHeightfield(m_chf);
			m_chf = 0;
			rcFreeContourSet(m_cset);
			m_cset = 0;
			rcFreePolyMesh(m_pmesh);
			m_pmesh = 0;
			rcFreePolyMeshDetail(m_dmesh);
			m_dmesh = 0;
			dtFreeNavMesh(m_navMesh);
			m_navMesh = 0;
		}

		//Detour Stuff
		dtQueryFilter m_filter;
		float m_polyPickExt[3];
	};
}
