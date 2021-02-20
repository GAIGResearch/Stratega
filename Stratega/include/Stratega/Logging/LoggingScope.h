#pragma once
#include <string>

namespace SGA
{
	class LoggingScope
	{
	public:
		LoggingScope(std::string&& scopeName);
		LoggingScope(const LoggingScope&) = delete;
		LoggingScope(LoggingScope&&) = delete;
		LoggingScope& operator=(LoggingScope other) = delete;
		LoggingScope& operator=(LoggingScope&& other) = delete;
		~LoggingScope();

		std::string& getName() { return scopeName; }
	private:
		std::string scopeName;
	};
}
