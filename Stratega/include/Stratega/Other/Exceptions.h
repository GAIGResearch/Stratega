#pragma once

namespace SGA
{
	struct warning : public std::exception
	{
	public:
		warning(const std::string msg) : std::exception(msg.c_str())
		{
		}
	};
}