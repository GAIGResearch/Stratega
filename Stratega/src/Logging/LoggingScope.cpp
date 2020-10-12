#include <Logging/LoggingScope.h>
#include <Logging/Log.h>

namespace SGA
{
	LoggingScope::LoggingScope(std::string&& scopeName)
		: scopeName(std::move(scopeName))
	{
		Log::loggingScopes.emplace_back(this);
	}

	LoggingScope::~LoggingScope()
	{
		Log::loggingScopes.pop_back();
	}
}