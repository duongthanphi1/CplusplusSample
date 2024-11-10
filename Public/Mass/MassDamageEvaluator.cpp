
#include "MassDamageEvaluator.h"

#include "MassAIBehaviorTypes.h"
#include "MassDamageType.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "MassDamageSubsystem.h"
#include "MassStateTreeExecutionContext.h"


bool FMMassDamageEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(DamageSubsystemHandle);

	return true;
}

void FMMassDamageEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	UMassDamageSubsystem& DamageSystem = Context.GetExternalData(DamageSubsystemHandle);
	
	const FMassDamageType* DamageResult = DamageSystem.LastDamageActor(static_cast<FMassStateTreeExecutionContext&>(Context).GetEntity());

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bGotDamage = false;
	/*//UE_LOG(LogTemp,Warning,TEXT("Delta time %f"), DeltaTime);
	if (DamageResult != nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("Find Damage Result %d"), static_cast<FMassStateTreeExecutionContext&>(Context).GetEntity().Index);
	}*/
	
	
	//UE_LOG(LogTemp,Warning,TEXT("Tick Damage %f from %d"),CurrentTime,static_cast<FMassStateTreeExecutionContext&>(Context).GetEntity().Index);
	if (DamageResult != nullptr)
	{
		const UWorld* World = Context.GetWorld();
		check(World);
		const double CurrentTime = World->GetTimeSeconds();
		// @todo: This is a bit of a kludge to expose an event to StateTree.
		
		const double TimeSinceHit = CurrentTime - DamageResult->HitTime;
		constexpr double HitEventDuration = 0.5;
		//UE_LOG(LogTemp,Error,TEXT("Time Since DAMAGE  %f"), TimeSinceHit);
		if (TimeSinceHit < HitEventDuration)
		{
			
			MASSBEHAVIOR_LOG(VeryVerbose, TEXT("Got Damage"));
			InstanceData.bGotDamage = true;
			InstanceData.LastCauserEntity = DamageResult->OtherEntity;
		
		}
	}
	
	
};
