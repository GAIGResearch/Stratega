#pragma once
#include <string>
#include <vector>

// Taken and adapted from https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c
// @author iain
class InputParser
{
public:
    InputParser(int argc, char** argv)
	{
    	// Convert arguments into strings
        for (auto i = 1; i < argc; ++i)
        {
            this->tokens.emplace_back(argv[i]);
        }
    }
	
    const std::string& getCmdOption(const std::string& option) const
	{
        auto it = std::find(this->tokens.begin(), this->tokens.end(), option);
        if (it != this->tokens.end() && ++it != this->tokens.end())
        {
            return *it;
        }
    	
        static const std::string EMPTY_STRING("");
        return EMPTY_STRING;
    }
	
    bool cmdOptionExists(const std::string& option) const
	{
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

	template<typename T>
    T getCmdOption(const std::string& option, T defaultValue)
    {
        if (!cmdOptionExists(option))
            return defaultValue;

        std::stringstream ss(getCmdOption(option));
        T val;
        ss >> val;
        return val;
    }
private:
    std::vector<std::string> tokens;
};