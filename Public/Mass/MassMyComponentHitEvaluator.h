// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MassComponentHitSubsystem.h"
#include "MassStateTreeTypes.h"

#include "MassMyComponentHitEvaluator.generated.h"

class UMassMyComponentHitSubsystem;

/**
 * Evaluator to extract last hit from the MassComponentHitSubsystem and expose it for tasks and transitions
 */

USTRUCT()
struct RPGGAME_API FMassMyComponentHitEvaluatorInstanceData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Output)
	bool bGotHit = false;

	UPROPERTY(VisibleAnywhere, Category = Output)
	FMassEntityHandle LastHitEntity;
};

USTRUCT(meta = (DisplayName = "My Mass ComponentHit Eval"))
struct RPGGAME_API FMassMyComponentHitEvaluator : public FMassStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMassMyComponentHitEvaluatorInstanceData;
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<UMassComponentHitSubsystem> ComponentHitSubsystemHandle;
};
