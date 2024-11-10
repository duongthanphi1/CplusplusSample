// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassDamageType.h"
#include "MassSignalSubsystem.h"
#include "MassSubsystemBase.h"

#include "MassDamageSubsystem.generated.h"

class UMassAgentSubsystem;
class UGSCCoreComponent;
/**
 * 
 */
UCLASS()
class RPGGAME_API UMassDamageSubsystem : public UMassTickableSubsystemBase
{
	GENERATED_BODY()

public:
	
	const FMassDamageType* LastDamageActor(const FMassEntityHandle Entity) const;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void RegisterForHealthChange(const FMassEntityHandle Entity, class UGSCCoreComponent& CoreComponent);
	void UnregisterForHealthChange(FMassEntityHandle Entity, class UGSCCoreComponent& CoreComponent);

	UFUNCTION()
	void OnDamageCallBack(AActor* Causer, UGSCCoreComponent* OtherCoreComp,UGSCCoreComponent* PlayerCoreComp);
	
	
	UPROPERTY()
	TObjectPtr<UMassAgentSubsystem> AgentSubsystem;

	UPROPERTY()
	TObjectPtr<UMassSignalSubsystem> SignalSubsystem;

	UPROPERTY()
	TMap<TObjectPtr<UActorComponent>, FMassEntityHandle> ComponentToEntityMap;

	UPROPERTY()
	TMap<FMassEntityHandle, TObjectPtr<UActorComponent>> EntityToComponentMap;

	UPROPERTY()
	TMap<FMassEntityHandle,FMassDamageType> DamageResult;
	
	

};
