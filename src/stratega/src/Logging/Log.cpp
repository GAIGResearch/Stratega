#include <Stratega/Logging/Log.h>
#include <Stratega/Logging/LoggingScope.h>

namespace SGA
{
	std::unique_ptr<SGA::FileLogger> defaultLogger;
	std::vector<SGA::LoggingScope*> loggingScopes;
	std::mutex loggingMutex;

	SGA::FileLogger& getDefaultLogger()
	{
		if (defaultLogger == nullptr)
			defaultLogger = std::make_unique<SGA::FileLogger>("./sgaLog.log");

		return *defaultLogger;
	}

	void setDefaultLogger(std::unique_ptr<SGA::FileLogger> logger)
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