// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/SimpleRandomMovementProcessor.h"

#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTypes.h"
#include "MassExecutionContext.h"
#include "Mass/SimpleRandomMovementFragment.h"
#include "MassMovement/Public/MassMovementFragments.h"
//#include "Mass/SimpleRandomMovementTrait.h"

USimpleRandomMovementProcessor::USimpleRandomMovementProcessor() : EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void USimpleRandomMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager,Context,([this](FMassExecutionContext& Context)
	{
		const TArrayView<FTransformFragment> TransformList =Context.GetMutableFragmentView<FTransformFragment>();
		const TArrayView<FSimpleRandomMovementFragment> SRMList =Context.GetMutableFragmentView<FSimpleRandomMovementFragment>();
		const float WorldDeltaTime = Context.GetDeltaTimeSeconds();
		
		

		for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
		{
			FTransform& Transform = TransformList[EntityIndex].GetMutableTransform();
			FVector& MoveTarget = SRMList[EntityIndex].Target;

			FVector CurrentLocation = Transform.GetLocation();
			FVector Target = MoveTarget - CurrentLocation;

			FMassEntityHandle Entity = Context.GetEntity(EntityIndex);
			UE_LOG(LogTemp, Error, TEXT("infor : %s"),*Entity.DebugGetDescription());
			
			if (Target.Size() < 20.f)
			{
				MoveTarget = FVector(FMath::RandRange(-1.f,1.f) * 1000.f,FMath::RandRange(-1.f,1.f) * 1000.f, CurrentLocation.Z);
			}
			else
			{
				Transform.SetLocation(CurrentLocation+Target.GetSafeNormal() * 400.f * WorldDeltaTime);
			}
			
		}
		
	}));
}

void USimpleRandomMovementProcessor::ConfigureQueries()
{
	
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FSimpleRandomMovementFragment>(EMassFragmentAccess::ReadWrite);
	//EntityQuery.AddRequirement<fradius>()
	
}



