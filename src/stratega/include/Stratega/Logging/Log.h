#pragma once
#include <Stratega/Logging/FileLogger.h>
#include <Stratega/Logging/LoggingScope.h>

#include <mutex>

namespace SGA
{
	extern std::unique_ptr<SGA::FileLogger> defaultLogger;
	extern std::vector<SGA::LoggingScope*> loggingScopes;
	extern std::mutex loggingMutex;

	SGA::FileLogger& getDefaultLogger();
	void setDefaultLogger(std::unique_ptr<SGA::FileLogger> logger);
	std::string getLoggingScopeKey(const std::string& lastKey);
	
	template<typename T>
	void logSingleValue(const std::string& key, const T& value)
	{
		std::lock_guard<std::mutex> logGuard(loggingMutex);
		getDefaultLogger().logSingleValue(getLoggingScopeKey(key), value);
	}

	template<typename T>
	void logValue(const std::string& key, const T& value)
	{
		std::lock_guard<std::mutex> logGuard(loggingMutex);
		getDefaultLogger().logValue(getLoggingScopeKey(key), value);
	}
}
