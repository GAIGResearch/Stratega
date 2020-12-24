#include <ForwardModel/FunctionFactory.h>
#include <ForwardModel/Condition.h>
#include <ForwardModel/Effect.h>

namespace SGA
{
	FunctionFactory<Condition>& FunctionFactory<Condition>::getDefaultFactory()
	{
		static FunctionFactory<Condition> factory;
		factory.registerFunction<HasResource>("HasResource");
		factory.registerFunction<SamePlayer>("SamePlayer");
		
		return factory;
	}

	FunctionFactory<Effect>& FunctionFactory<Effect>::getDefaultFactory()
	{
		static FunctionFactory<Effect> factory;
		factory.registerFunction<AddToResource>("AddResource");
		factory.registerFunction<Attack>("Attack");
		factory.registerFunction<RemoveFromResource>("RemoveFromResource");
		factory.registerFunction<Move>("Move");
		factory.registerFunction<SpawnUnit>("SpawnUnit");

		return factory;
	}
}
