#pragma once
#include "DetourNavMesh.h"
#include "Navigation.h"


namespace SGA
{
	class Path
	{
	public:

		//Definition of a path
		float m_straightPath[MAX_POLYS * 3];
		int m_nstraightPath = 0;
		
		unsigned char m_straightPathFlags[MAX_POLYS];
		dtPolyRef m_straightPathPolys[MAX_POLYS];
		
		int m_straightPathOptions = 0;
		
		int  currentPathIndex = 0;

		bool isEmpty() const { return m_nstraightPath == 0; }
		
	};
	
}
