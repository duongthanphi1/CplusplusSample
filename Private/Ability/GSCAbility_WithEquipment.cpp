// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GSCAbility_WithEquipment.h"

#include "Equipment/EquipmentInstance.h"
#include "Invventory/InventoryItemInstance.h"
#include "Templates/Casts.h"
#include "DidItHitActorComponent.h"
#include "Components/GSCCoreComponent.h"
#include "GameFramework/Character.h"


UGSCAbility_WithEquipment::UGSCAbility_WithEquipment()
{
	
}




void UGSCAbility_WithEquipment::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	WeaponTraceComponent = AvatarActor->FindComponentByClass<UDidItHitActorComponent>();
	if (WeaponTraceComponent)
	{
		if (WeaponTraceComponent->OnItemAdded.IsBound())
		{
			
			WeaponTraceComponent->OnItemAdded.AddDynamic(this,&UGSCAbility_WithEquipment::OnHitActor);
			WeaponTraceComponent->OnItemAdded.AddDynamic(this,&UGSCAbility_WithEquipment::OnDamageHitActor);
		}
		
	}
	
}

void UGSCAbility_WithEquipment::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	const AActor* AvatarActor = GetAvatarActorFromActorInfo();
	WeaponTraceComponent = AvatarActor->FindComponentByClass<UDidItHitActorComponent>();
	if (WeaponTraceComponent)
	{
		
		WeaponTraceComponent->OnItemAdded.RemoveDynamic(this,&UGSCAbility_WithEquipment::OnHitActor);
		WeaponTraceComponent->OnItemAdded.RemoveDynamic(this,&UGSCAbility_WithEquipment::OnDamageHitActor);
		
	}
	//UE_LOG(LogTemp,Warning,TEXT("Remove Delegate"));
}

UEquipmentInstance* UGSCAbility_WithEquipment::GetContactEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

UInventoryItemInstance* UGSCAbility_WithEquipment::GetContactItem() const
{
	if (UEquipmentInstance* EquipmentInfor = GetContactEquipment())
	{
		return Cast<UInventoryItemInstance>(EquipmentInfor->GetInstigator());
	}
	return nullptr;
}

void UGSCAbility_WithEquipment::OnHitActor_Implementation(FHitResult LastHitActor)
{
	
}

void UGSCAbility_WithEquipment::OnDamageHitActor(FHitResult LastHitActor) 
{
	ACharacter* Enemy = Cast<ACharacter>(LastHitActor.GetActor());
	if (Enemy != nullptr)
	{
		UGSCCoreComponent* PlayerCoreComponent = GetAvatarActorFromActorInfo()->FindComponentByClass<UGSCCoreComponent>();
		if (PlayerCoreComponent)
		{
			PlayerCoreComponent->OnTakeDamage.Broadcast(GetAvatarActorFromActorInfo(),Enemy->FindComponentByClass<UGSCCoreComponent>(),PlayerCoreComponent);
		}
	}
}




