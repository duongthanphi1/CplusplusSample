// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "MassStateTreeTypes.h"
#include "UObject/Object.h"
#include "MassInteractionEvaluator.generated.h"

class UMassCrownInteractionSubsystem;

USTRUCT()
struct RPGGAME_API FMassInteracEvaluatorInstancedData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Output)
	bool bGotInteract = false;

	UPROPERTY(VisibleAnywhere, Category = Output)
	FMassEntityHandle LastCauserEntity;
};

USTRUCT(meta = (DisplayName = "Mass Crown Interaction Eval"))
struct RPGGAME_API FMassnteractionEvaluator : public FMassStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMassInteracEvaluatorInstancedData;
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<UMassCrownInteractionSubsystem> InteractionSubsystemHandle;
};