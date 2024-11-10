// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GSCAbility_WithEquipment_BaseCombo.h"

#include "DidItHitActorComponent.h"
#include "GASCompanion/Public/Abilities/GSCBlueprintFunctionLibrary.h"
#include "GASCompanion/Public/Components/GSCComboManagerComponent.h"
#include "GASCompanion/Public/Abilities/Tasks/GSCTask_PlayMontageWaitForEvent.h"




UGSCAbility_WithEquipment_BaseCombo::UGSCAbility_WithEquipment_BaseCombo()
{
}
void UGSCAbility_WithEquipment_BaseCombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	

	ComboManagerComponent = UGSCBlueprintFunctionLibrary::GetComboManagerComponent(AvatarActor);
	if (!ComboManagerComponent)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	ComboManagerComponent->IncrementCombo();

	UAnimMontage* Montage = GetNextComboMontage();
	

	UGSCTask_PlayMontageWaitForEvent* Task = UGSCTask_PlayMontageWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, Montage, WaitForEventTag, Rate, NAME_None, true, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGSCAbility_WithEquipment_BaseCombo::OnMontageCompleted);
	Task->OnCompleted.AddDynamic(this, &UGSCAbility_WithEquipment_BaseCombo::OnMontageCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGSCAbility_WithEquipment_BaseCombo::OnMontageCancelled);
	Task->OnCancelled.AddDynamic(this, &UGSCAbility_WithEquipment_BaseCombo::OnMontageCancelled);
	Task->EventReceived.AddDynamic(this, &UGSCAbility_WithEquipment_BaseCombo::OnEventReceived);
	Task->ReadyForActivation();
}

void UGSCAbility_WithEquipment_BaseCombo::OnMontageCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGSCAbility_WithEquipment_BaseCombo::OnMontageCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGSCAbility_WithEquipment_BaseCombo::OnEventReceived_Implementation(const FGameplayTag EventTag, const FGameplayEventData EventData)
{
	ApplyEffectContainer(EventTag, EventData);
	
}

UAnimMontage* UGSCAbility_WithEquipment_BaseCombo::GetNextComboMontage()
{
	if (!ComboManagerComponent)
	{
		return nullptr;
	}

	int32 ComboIndex = ComboManagerComponent->ComboIndex;

	if (ComboIndex >= Montages.Num())
	{
		ComboIndex = 0;
	}

	return Montages.IsValidIndex(ComboIndex) ? Montages[ComboIndex] : nullptr;
}




