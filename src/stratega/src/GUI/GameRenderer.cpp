#include <cassert>

#include <Stratega/GUI/GameRenderer.h>
#include <Stratega/GUI/RTSGameRenderer.h>
#include <Stratega/GUI/TBSGameRenderer.h>
#include <Stratega/NewGUI/NewTBSGameRenderer.h>

namespace SGA
{
	std::unique_ptr<GameRenderer> createRenderer(const GameType& type, SGA::Vector2i& resolution)
	{
		if (type == GameType::TBS)
		{
			return std::make_unique<NewTBSGameRenderer>(resolution);
		}
		if (type == GameType::RTS)
		{
			return std::make_unique<RTSGameRenderer>(resolution);
		}

		assert(false);
		return nullptr;
	}

}
