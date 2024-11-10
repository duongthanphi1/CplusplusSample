// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Object.h"
#include "GASCompanion/Public/Abilities/GSCGameplayAbility.h"

#include "GSCAbility_WithEquipment.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewHit, FHitResult, LastHitActor);

class UEquipmentInstance;
class UInventoryItemInstance;
class UDidItHitActorComponent;
/**
 * 
 */
UCLASS()
class RPGGAME_API UGSCAbility_WithEquipment : public UGSCGameplayAbility
{
	GENERATED_BODY()
public:
	UGSCAbility_WithEquipment();

	UPROPERTY()
	TObjectPtr<UDidItHitActorComponent> WeaponTraceComponent;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UFUNCTION()
	UEquipmentInstance* GetContactEquipment() const;

	UFUNCTION()
	UInventoryItemInstance* GetContactItem() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnHitActor(FHitResult LastHitActor);


	UFUNCTION()
	void OnDamageHitActor(FHitResult LastHitActor) ;
	
	
};
