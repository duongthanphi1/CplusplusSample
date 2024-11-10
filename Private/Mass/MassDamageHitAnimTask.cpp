// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/MassDamageHitAnimTask.h"
// Copyright Epic Games, Inc. All Rights Reserved.



#include "MassSignalSubsystem.h"
#include "MassStateTreeExecutionContext.h"
#include "MassCrowdAnimationTypes.h"
#include "MassCommandBuffer.h"
#include "MassExecutionContext.h"
#include "Animation/AnimMontage.h"
#include "MassNavigationFragments.h"
#include "MassZoneGraphNavigationUtils.h"
#include "StateTreeLinker.h"


FDamageHitAnimTask::FDamageHitAnimTask()
{
}

bool FDamageHitAnimTask::Link(FStateTreeLinker& Linker)
{
	Linker.LinkExternalData(MassSignalSubsystemHandle);
	Linker.LinkExternalData(MontageRequestHandle);
	Linker.LinkExternalData(TransformHandle);
	Linker.LinkExternalData(MoveTargetHandle);

	return true;
}

EStateTreeRunStatus FDamageHitAnimTask::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	UE_LOG(LogTemp,Warning,TEXT("EnterState"));
	const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(Context);

	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	InstanceData.Time = 0.f;

	UE::CrowdInteractionAnim::FRequest AnimRequest;
	
	FTransformFragment TransformFragment = Context.GetExternalData(TransformHandle);
	FContextualAnimQueryResult& ContextualAnimQueryResult = AnimRequest.QueryResult;

	if (InstanceData.TargetEntity.IsSet())
	{
		
		const FTransformFragment* PlayerTransformFragment = MassContext.GetEntityManager().GetFragmentDataPtr<FTransformFragment>(InstanceData.TargetEntity);
		const FTransform& PlayerTransform = PlayerTransformFragment->GetTransform();

		FVector PlayerToEntityDir = PlayerTransform.GetLocation()-TransformFragment.GetTransform().GetLocation(); 
		float FDotProduct = FVector::DotProduct(UE::Math::TVector<double>(PlayerToEntityDir.Normalize()),TransformFragment.GetTransform().GetRotation().GetForwardVector());
		float RDotProduct = FVector::DotProduct(UE::Math::TVector<double>(PlayerToEntityDir.Normalize()),TransformFragment.GetTransform().GetRotation().GetRightVector());
		//UE_LOG(LogTemp,Warning,TEXT("Yaw : %f ; Pitch : %f"),TransformFragment.GetMutableTransform().GetRotation().Z,PlayerTransform.GetRotation().Z);
		//UE_LOG(LogTemp,Warning,TEXT("S1 %d s2 %d"),MassContext.GetEntity().Index,InstanceData.TargetEntity.Index);
		UE_LOG(LogTemp,Warning,TEXT("FDP : %f ; RDP : %f"),FDotProduct,RDotProduct);
		if (FMath::Abs(RDotProduct) < .1f)
		{
			if (FDotProduct < 0.f)
			{
				ContextualAnimQueryResult.Animation = HitForward;
			}
			else
			{
				ContextualAnimQueryResult.Animation = HitBackward;
			}
			
		}
		else if ( FMath::Abs(RDotProduct) > .1f && FMath::Abs(RDotProduct) < .5f)
		{
			ContextualAnimQueryResult.Animation = HitLeft;
		}
		else
		{
			ContextualAnimQueryResult.Animation = HitRight;
		}
	}
	
	// If we didn't find a proper contextual anim, or it was not set, use a simple montage instead
	/*if (!ContextualAnimQueryResult.Animation.IsValid())
	{
		ContextualAnimQueryResult.Animation = FallbackMontage;
		UE_LOG(LogTemp,Warning,TEXT("didn't find a proper contextual anim"));
	}*/

	InstanceData.ComputedDuration = InstanceData.Duration;
	
	if (const UAnimMontage* Montage = ContextualAnimQueryResult.Animation.Get())
	{
		// Only override movement mode if we have root motion
		if (Montage->HasRootMotion())
		{
			const UWorld* World = Context.GetWorld();
			checkf(World != nullptr, TEXT("A valid world is expected from the execution context"));

			FMassMoveTargetFragment& MoveTarget = Context.GetExternalData(MoveTargetHandle);
			MoveTarget.CreateNewAction(EMassMovementAction::Animate, *World);
			if (!UE::MassNavigation::ActivateActionAnimate(*World, Context.GetOwner(), MassContext.GetEntity(), MoveTarget))
			{
				return EStateTreeRunStatus::Failed;
			}
		}
		
		// Grab the task duration from the montage.
		InstanceData.ComputedDuration = Montage->GetPlayLength();
		UE_LOG(LogTemp,Warning,TEXT("Montage Duration : %f"),InstanceData.ComputedDuration);
		// Use existing fragment or push one
		FMassMontageFragment* MontageFragment = MassContext.GetExternalDataPtr(MontageRequestHandle);
		if (MontageFragment != nullptr)
		{
			MontageFragment->Request(AnimRequest);
		}
		else
		{
			FMassMontageFragment MontageData;
			MontageData.Request(AnimRequest);
			MassContext.GetEntitySubsystemExecutionContext().Defer().PushCommand<FMassCommandAddFragmentInstances>(MassContext.GetEntity(), MontageData);
		}
	}

	// A Duration <= 0 indicates that the task runs until a transition in the state tree stops it.
	// Otherwise we schedule a signal to end the task.
	if (InstanceData.ComputedDuration > 0.0f)
	{
		UMassSignalSubsystem& MassSignalSubsystem = MassContext.GetExternalData(MassSignalSubsystemHandle);
		MassSignalSubsystem.DelaySignalEntity(UE::Mass::Signals::ContextualAnimTaskFinished, MassContext.GetEntity(), InstanceData.ComputedDuration);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FDamageHitAnimTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	InstanceData.Time += DeltaTime;
	return InstanceData.ComputedDuration <= 0.0f ? EStateTreeRunStatus::Running : (InstanceData.Time < InstanceData.ComputedDuration ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Succeeded);
}
