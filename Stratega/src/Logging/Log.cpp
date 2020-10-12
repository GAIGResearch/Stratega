#include <Logging/Log.h>
#include <Logging/LoggingScope.h>

namespace SGA::Log
{
	FileLogger& getDefaultLogger()
	{
		if (defaultLogger == nullptr)
			defaultLogger = std::make_unique<FileLogger>("./sgaLog.log");

		return *defaultLogger;
	}

	void setDefaultLogger(std::unique_ptr<FileLogger> logger)
	{
		defaultLogger = std::move(logger);
	}

	std::string getLoggingScopeKey(const std::string& lastKey)
	{
		std::string key;
		for (const auto& scope : loggingScopes)
		{
			key += scope->getName() + "/";
		}
		return key + lastKey;
	}
}