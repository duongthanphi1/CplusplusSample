// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/CustomSmartObjectMassBehaviorDefinition.h"

#include "MassCommonFragments.h"

void UCustomSmartObjectMassBehaviorDefinition::Activate(FMassCommandBuffer& CommandBuffer,
                                                        const FMassBehaviorEntityContext& EntityContext) const
{
	Super::Activate(CommandBuffer, EntityContext);
	
	UE_LOG(LogTemp,Error, TEXT("test OBJECT"));
}

void UCustomSmartObjectMassBehaviorDefinition::Deactivate(FMassCommandBuffer& CommandBuffer,
	const FMassBehaviorEntityContext& EntityContext) const
{
	Super::Deactivate(CommandBuffer, EntityContext);
}
