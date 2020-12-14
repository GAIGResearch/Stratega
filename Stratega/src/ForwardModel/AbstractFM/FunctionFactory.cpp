#include <ForwardModel/AbstractFM/FunctionFactory.h>
#include <ForwardModel/AbstractFM/Condition.h>
#include <ForwardModel/AbstractFM/Effect.h>

namespace SGA
{
	FunctionFactory<Condition>& FunctionFactory<Condition>::getDefaultFactory()
	{
		static FunctionFactory<Condition> factory;
		factory.registerFunction<HasResource>("HasResource");
		
		return factory;
	}

	FunctionFactory<Effect>& FunctionFactory<Effect>::getDefaultFactory()
	{
		static FunctionFactory<Effect> factory;
		factory.registerFunction<AddToResource>("AddResource");

		return factory;
	}
}
