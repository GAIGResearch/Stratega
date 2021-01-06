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
		factory.registerFunction<InRange>("InRange");
		factory.registerFunction<IsWalkable>("IsWalkable");
		
		return factory;
	}

	FunctionFactory<Effect>& FunctionFactory<Effect>::getDefaultFactory()
	{
		static FunctionFactory<Effect> factory;
		factory.registerFunction<ModifyResource>("ModifyResource");
		factory.registerFunction<Attack>("Attack");
		factory.registerFunction<Move>("Move");
		factory.registerFunction<SpawnUnit>("Spawn");
		factory.registerFunction<SetToMaximum>("SetToMaximum");

		return factory;
	}
}
