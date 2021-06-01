#include <Stratega/ForwardModel/FunctionFactory.h>
#include <Stratega/ForwardModel/Condition.h>
#include <Stratega/ForwardModel/Effect.h>

namespace SGA
{

	template<>
	FunctionFactory<Condition>& FunctionFactory<Condition>::getDefaultFactory()
	{
		static FunctionFactory<Condition> factory;
		factory.registerFunction<HasEntity>("HasEntity");
		factory.registerFunction<NoHasEntity>("NoHasEntity");
		factory.registerFunction<ResourceGreater>("ResourceGreater");
		factory.registerFunction<ResourceLower>("ResourceLower");
		factory.registerFunction<SamePlayer>("SamePlayer");
		factory.registerFunction<InRange>("InRange");
		factory.registerFunction<IsTile>("IsTileType");
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
		factory.registerFunction<ChangeResource>("ChangeResource");
		factory.registerFunction<ModifyResource>("ModifyResource");
		factory.registerFunction<Attack>("Attack");
		factory.registerFunction<AttackProbability>("AttackProbability");
		factory.registerFunction<Move>("Move");
		factory.registerFunction<SpawnEntity>("SpawnEntity");
		factory.registerFunction<SpawnEntityGrid>("SpawnEntityGrid");
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
