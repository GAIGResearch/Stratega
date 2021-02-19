#include <ForwardModel/FunctionFactory.h>
#include <ForwardModel/Condition.h>
#include <ForwardModel/Effect.h>

namespace SGA
{

	template<>
	FunctionFactory<Condition>& FunctionFactory<Condition>::getDefaultFactory()
	{
		static FunctionFactory<Condition> factory;
		factory.registerFunction<HasResource>("HasResource");
		factory.registerFunction<SamePlayer>("SamePlayer");
		factory.registerFunction<InRange>("InRange");
		factory.registerFunction<IsWalkable>("IsWalkable");
		factory.registerFunction<IsPlayerEntity>("IsPlayerEntity");
		factory.registerFunction<IsResearched>("HasResearched");
		factory.registerFunction<CanResearch>("CanResearch");
		factory.registerFunction<CanSpawnCondition>("CanSpawn");
		factory.registerFunction<CanAfford>("CanAfford");
		factory.registerFunction<HasElapsedTime>("HasElapsedTick");
		
		return factory;
	}


	template<>
	FunctionFactory<Effect>& FunctionFactory<Effect>::getDefaultFactory()
	{
		static FunctionFactory<Effect> factory;
		factory.registerFunction<ModifyResource>("ModifyResource");
		factory.registerFunction<Attack>("Attack");
		factory.registerFunction<Move>("Move");
		factory.registerFunction<SpawnUnit>("Spawn");
		factory.registerFunction<SpawnEntity>("SpawnEntity");
		factory.registerFunction<SetToMaximum>("SetToMaximum");
		factory.registerFunction<TransferEffect>("Transfer");
		factory.registerFunction<ChangeOwnerEffect>("ChangeOwner");
		factory.registerFunction<RemoveEntityEffect>("Remove");
		factory.registerFunction<ResearchTechnology>("Research");
		factory.registerFunction<SpawnEntityRandom>("SpawnRandom");
		factory.registerFunction<PayCostEffect>("PayCost");
		
		return factory;
	}
}
