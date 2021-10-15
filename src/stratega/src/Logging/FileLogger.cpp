#include <Stratega/Logging/FileLogger.h>
#include <iostream>
#include <yaml-cpp/emitter.h>

namespace SGA
{
	FileLogger::FileLogger(const std::string& filePath)
		: outputStream(filePath), loggedScalars(YAML::NodeType::Map)
	{
		// Note that we keep the file open the whole time the logger object exists
		// Even though we only write to it in the destructor, we do this to later change to an more memory efficient implementation.
	}

	FileLogger::~FileLogger()
	{
		try
		{
			if (loggedScalars.size() == 0)
				return;
			
			YAML::Emitter emitter;
			emitter << loggedScalars;
			outputStream << emitter.c_str();
		}
		catch (...)
		{
		}
	}

	void FileLogger::flush()
	{
		YAML::Emitter emitter;
		emitter << loggedScalars;
		outputStream << emitter.c_str();
		outputStream << std::endl;
		outputStream.flush();

		loggedScalars = YAML::Node(YAML::NodeType::Map);
	}
}