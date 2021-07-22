#pragma once
#include <functional>
#include <memory>
#include <Stratega/ForwardModel/FunctionParameter.h>

namespace SGA
{
	template<typename Function>
	class FunctionFactory
	{
	public:
		typedef std::function<Function* (const std::string& code, const std::vector<FunctionParameter>& params)> FnAllocator;
		
		static FunctionFactory<Function>& get()
		{
			static FunctionFactory<Function>& instance = getDefaultFactory();
			return instance;
		}

		template<typename T>
		bool registerFunction(const std::string& name)
		{
			if (registeredFunctions.find(name) != registeredFunctions.end())
				return false;

			registeredFunctions.emplace(
				name,
				[](const std::string& code, const std::vector<FunctionParameter>& params) {return static_cast<Function*>(new T(code, params)); }
			);
			return true;
		}

		std::unique_ptr<Function> createFunction(const std::string& code, const std::string& name, const std::vector<FunctionParameter>& params)
		{
			if (registeredFunctions.find(name) == registeredFunctions.end())
				return nullptr;
			
			return std::unique_ptr<Function>(registeredFunctions.at(name)(code, params));
		}

		// This class should not be copied
		FunctionFactory(const FunctionFactory&) = delete;
		FunctionFactory(FunctionFactory&&) = delete;
		FunctionFactory& operator=(const FunctionFactory& other) = delete;
		FunctionFactory& operator=(FunctionFactory&& other) noexcept = delete;

	private:
		std::unordered_map<std::string, FnAllocator> registeredFunctions;

		FunctionFactory() = default;
		~FunctionFactory() = default;

		/// <summary>
		/// Returns a factory-reference with all functions registered
		/// Called once to initialize the get() method, do not call more than once!
		/// </summary>
		static FunctionFactory<Function>& getDefaultFactory();
	};
}
