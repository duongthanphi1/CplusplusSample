// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassDamageType.h"
#include "MassSubsystemBase.h"
#include "MassCrownInteractionSubsystem.generated.h"

class UMassAgentSubsystem;
class UMassInteractionComponent;
class UMassSignalSubsystem;
/**
 * 
 */
UCLASS()
class RPGGAME_API UMassCrownInteractionSubsystem : public UMassTickableSubsystemBase
{
	GENERATED_BODY()

public:
	const FMassInteractionType* LastInteracActor(const FMassEntityHandle Entity) const;
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void RegisterForInteration(const FMassEntityHandle Entity, class UMassInteractionComponent& CoreComponent);
	void UnregisterForInteration(FMassEntityHandle Entity, class UMassInteractionComponent& CoreComponent);

	UFUNCTION()
	void OnBumpCallBack(UMassInteractionComponent*OtherComponent,UMassInteractionComponent*PlayerComponent);
	
	
	UPROPERTY()
	TObjectPtr<UMassAgentSubsystem> AgentSubsystem;

	UPROPERTY()
	TObjectPtr<UMassSignalSubsystem> SignalSubsystem;

	UPROPERTY()
	TMap<TObjectPtr<UActorComponent>, FMassEntityHandle> ComponentToEntityMap;

	UPROPERTY()
	TMap<FMassEntityHandle, TObjectPtr<UActorComponent>> EntityToComponentMap;

	UPROPERTY()
	TMap<FMassEntityHandle,FMassInteractionType> InteractResults;
	
	
};
