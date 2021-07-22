#pragma once
#include <fstream>
#include <unordered_map>
#include <yaml-cpp/yaml.h>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/node/node.h>

namespace SGA
{
	class FileLogger
	{
	public:
		explicit FileLogger(const std::string& filePath);
		~FileLogger();

		void flush();

		template<typename T>
		void logValue(const std::string& key, const T& value)
		{
			YAML::Node node = getNodeFromKey(key, loggedScalars, 0, YAML::NodeType::Sequence);
			node.push_back(value);
		}

		template<typename T>
		void logSingleValue(const std::string& key, const T& value)
		{
			YAML::Node node = getNodeFromKey(key, loggedScalars, 0, YAML::NodeType::Scalar);
			node = value;
		}

	private:
		std::ofstream outputStream;
		YAML::Node loggedScalars;

		// For Developers: This would be easier to read if it was written iteratively.
		// Problem is that YAML::Node is a reference type, and that makes it much harder to write it iteratively, since we can't use a single variable to represent our current node.
		static YAML::Node getNodeFromKey(const std::string& key, YAML::Node curNode, size_t keyPos, YAML::NodeType::value leafType)
		{
			size_t newPos = key.find("/", keyPos);
			if (newPos == std::string::npos)
			{
				// The last token is the key used to group the value
				auto lastToken = key.substr(keyPos, key.size() - keyPos);
				if (!curNode[lastToken] || curNode[lastToken].Type() != leafType)
				{
					// Note if the node already existed but wit a different type, it will be overwritten
					// We could also throw an exception...
					curNode[lastToken] = YAML::Node(leafType);
					curNode[lastToken].SetStyle(YAML::EmitterStyle::Flow);
				}
				return curNode[lastToken];
			}
			else
			{
				// This is not the last token, so it tells us where the value goes in the hierarchy
				auto token = key.substr(keyPos, newPos - keyPos);
				if (!curNode[token])
				{
					curNode[token] = YAML::Node(YAML::NodeType::Map);
				}

				return getNodeFromKey(key, curNode[token], newPos + 1, leafType);
			}
		}
	};
}