#include <Stratega/Logging/LoggingScope.h>
#include <Stratega/Logging/Log.h>

namespace SGA
{
	LoggingScope::LoggingScope(std::string&& scopeName)
		: scopeName(std::move(scopeName))
	{
		SGA::loggingScopes.emplace_back(this);
	}

	LoggingScope::~LoggingScope()
	{
		SGA::loggingScopes.pop_back();
	}
}