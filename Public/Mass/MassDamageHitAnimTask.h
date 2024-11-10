// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassStateTreeTypes.h"
#include "UObject/Object.h"
#include "MassDamageHitAnimTask.generated.h"

/**
 * 
 */
class UAnimMontage;

class UMassSignalSubsystem;
struct FMassMontageFragment; 
struct FTransformFragment;
struct FMassMoveTargetFragment;

USTRUCT()
struct FMassDamageAnimTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = Input, meta = (Optional))
	FMassEntityHandle TargetEntity;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	float Duration = 0.0f;

	UPROPERTY()
	float ComputedDuration = 0.0f;

	/** Accumulated time used to stop task if a montage is set */
	UPROPERTY()
	float Time = 0.f;
};

USTRUCT(meta = (DisplayName = "Mass Damage Hit Anim Task"))
struct FDamageHitAnimTask : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMassDamageAnimTaskInstanceData; 

	FDamageHitAnimTask();

protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<UMassSignalSubsystem> MassSignalSubsystemHandle;
	TStateTreeExternalDataHandle<FMassMontageFragment, EStateTreeExternalDataRequirement::Optional> MontageRequestHandle; 
	TStateTreeExternalDataHandle<FTransformFragment> TransformHandle;
	TStateTreeExternalDataHandle<FMassMoveTargetFragment> MoveTargetHandle;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	UAnimMontage* HitForward = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	UAnimMontage* HitBackward = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	UAnimMontage* HitLeft = nullptr;

	UPROPERTY(EditAnywhere, Category = Parameter)
	UAnimMontage* HitRight = nullptr;

	UPROPERTY()
	FHitResult HitResult;

	
	
};
