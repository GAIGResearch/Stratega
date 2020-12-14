#include <ForwardModel/AbstractFM/FunctionFactory.h>
#include <ForwardModel/AbstractFM/Precondition.h>
#include <ForwardModel/AbstractFM/Effect.h>

namespace SGA
{
	FunctionFactory<Precondition>& FunctionFactory<Precondition>::getDefaultFactory()
	{
		static FunctionFactory<Precondition> factory;
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
