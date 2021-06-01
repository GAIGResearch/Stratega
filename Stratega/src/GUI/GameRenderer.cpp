#include <cassert>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/GUI/RTSGameRenderer.h>
#include <Stratega/GUI/TBSGameRenderer.h>

namespace SGA
{
	std::unique_ptr<GameRenderer> createRenderer(GameType type)
	{
		if (type == GameType::TBS)
		{
			return std::make_unique<TBSGameRenderer>();
		}
		if (type == GameType::RTS)
		{
			return std::make_unique<RTSGameRenderer>();
		}

		assert(false);
		return nullptr;
	}

}
