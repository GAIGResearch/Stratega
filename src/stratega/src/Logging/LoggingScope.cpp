#include <Stratega/Logging/LoggingScope.h>
#include <Stratega/Logging/Log.h>

namespace SGA
{
	LoggingScope::LoggingScope(std::string&& newScopeNam)
		: scopeName(std::move(newScopeNam))
	{
		SGA::loggingScopes.emplace_back(this);
	}

	LoggingScope::~LoggingScope()
	{
		SGA::loggingScopes.pop_back();
	}
}