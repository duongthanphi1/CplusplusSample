// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GSCAbility_WithEquipment.h"
#include "UObject/Object.h"
#include "GSCAbility_WithEquipment_BaseCombo.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndAbility, FHitResult, LastHitActor);

class UDidItHitActorComponent;
class UGSCComboManagerComponent;
/**
 * 
 */
UCLASS()
class RPGGAME_API UGSCAbility_WithEquipment_BaseCombo : public UGSCAbility_WithEquipment
{
	GENERATED_BODY()
public:
	UGSCAbility_WithEquipment_BaseCombo();

protected:
	UPROPERTY()
	TObjectPtr<UGSCComboManagerComponent> ComboManagerComponent;
	
	

	/** List of animation montages you want to cycle through when activating this ability */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Montages")
	TArray<TObjectPtr<UAnimMontage>> Montages;

	/** Change to play the montage faster or slower */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	float Rate = 1.f;

	/** Any gameplay events matching this tag will activate the OnEventReceived callback and apply the gameplay effect containers for this ability */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	FGameplayTagContainer WaitForEventTag;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION(BlueprintNativeEvent)
	void OnEventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION(BlueprintPure, Category="GAS Companion|Ability|Melee")
	UAnimMontage* GetNextComboMontage();

	FOnGameplayAbilityEnded::FDelegate* OnAbilityEnded;

	
};
