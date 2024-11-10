// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/MassInteractionEvaluator.h"

#include "MassAIBehaviorTypes.h"
#include "MassStateTreeExecutionContext.h"
#include "StateTreeLinker.h"
#include "Mass/MassCrownInteractionSubsystem.h"
#include "Mass/MassDamageType.h"


bool FMassnteractionEvaluator::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(InteractionSubsystemHandle);

	return true;
}

void FMassnteractionEvaluator::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	UMassCrownInteractionSubsystem& InteractionSubsystem = Context.GetExternalData(InteractionSubsystemHandle);
	
	const FMassInteractionType* DamageResult = InteractionSubsystem.LastInteracActor(static_cast<FMassStateTreeExecutionContext&>(Context).GetEntity());

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	InstanceData.bGotInteract = false;
	
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
		
		const double TimeSinceHit = CurrentTime - DamageResult->InteractTime;
		constexpr double HitEventDuration = 0.5;
		
		if (TimeSinceHit < HitEventDuration)
		{
			
			
			MASSBEHAVIOR_LOG(VeryVerbose, TEXT("Got Interact"));
			InstanceData.bGotInteract = true;
			InstanceData.LastCauserEntity = DamageResult->OtherEntity;
		
		}
	}
	
	
};
