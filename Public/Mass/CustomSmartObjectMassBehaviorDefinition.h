// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassSmartObjectBehaviorDefinition.h"
#include "CustomSmartObjectMassBehaviorDefinition.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class RPGGAME_API UCustomSmartObjectMassBehaviorDefinition : public USmartObjectMassBehaviorDefinition
{
	GENERATED_BODY()

	virtual void Activate(FMassCommandBuffer& CommandBuffer, const FMassBehaviorEntityContext& EntityContext) const override;
	virtual void Deactivate(FMassCommandBuffer& CommandBuffer, const FMassBehaviorEntityContext& EntityContext) const override;
};
