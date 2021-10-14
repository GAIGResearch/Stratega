#include <cassert>

#include <stratega/GUI/GameRenderer.h>
#include <stratega/GUI/RTSGameRenderer.h>
#include <stratega/GUI/TBSGameRenderer.h>

namespace SGA
{
	std::unique_ptr<GameRenderer> createRenderer(const GameType& type, SGA::Vector2f& resolution)
	{
		if (type == GameType::TBS)
		{
			return std::make_unique<TBSGameRenderer>(resolution);
		}
		if (type == GameType::RTS)
		{
			return std::make_unique<RTSGameRenderer>(resolution);
		}

		assert(false);
		return nullptr;
	}

}
