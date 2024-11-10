// Fill out your copyright notice in the Description page of Project Settings.


#include "PrimaryAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GASCompanion/Public/Components/GSCCoreComponent.h"

// Sets default values
UPrimaryAttributeSet::UPrimaryAttributeSet()
{
	// Default constructor
}

void UPrimaryAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // This is called whenever attributes change, so for max attributes we want to scale the current totals to match
    Super::PreAttributeChange(Attribute, NewValue);

    
    if (Attribute == GetMaxHealthAttribute())
    {
        AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
        return;
    }

    if (Attribute == GetMaxStaminaAttribute())
    {
        AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
        return;
    }

    if (Attribute == GetMaxManaAttribute())
    {
        AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
        return;
    }
}

void UPrimaryAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGSCAttributeSetExecutionData ExecutionData;
    GetExecutionDataFromMod(Data, ExecutionData);

    
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        HandleHealthAttribute(ExecutionData);
    }
    else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        HandleStaminaAttribute(ExecutionData);
    }
    else if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        HandleManaAttribute(ExecutionData);
    }
}

void UPrimaryAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, Health, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(UPrimaryAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
}

void UPrimaryAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, Health, OldHealth);
}

void UPrimaryAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, MaxHealth, OldMaxHealth);
}

void UPrimaryAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, Mana, OldMana);
}

void UPrimaryAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, MaxMana, OldMaxMana);
}

void UPrimaryAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, Stamina, OldStamina);
}

void UPrimaryAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPrimaryAttributeSet, MaxStamina, OldMaxStamina);
}

void UPrimaryAttributeSet::HandleHealthAttribute(const FGSCAttributeSetExecutionData& ExecutionData)
{
    UGSCCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
    const float ClampMinimumValue = GetClampMinimumValueFor(GetHealthAttribute());

    SetHealth(FMath::Clamp(GetHealth(), ClampMinimumValue, GetMaxHealth()));

    if (TargetCoreComponent)
    {
        const float DeltaValue = ExecutionData.DeltaValue;
        const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
        TargetCoreComponent->HandleHealthChange(DeltaValue, SourceTags);
    }
}
void UPrimaryAttributeSet::HandleStaminaAttribute(const FGSCAttributeSetExecutionData& ExecutionData)
{
    UGSCCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
    const float ClampMinimumValue = GetClampMinimumValueFor(GetStaminaAttribute());

    SetStamina(FMath::Clamp(GetStamina(), ClampMinimumValue, GetMaxStamina()));

    if (TargetCoreComponent)
    {
        const float DeltaValue = ExecutionData.DeltaValue;
        const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
        TargetCoreComponent->HandleStaminaChange(DeltaValue, SourceTags);
    }
}

void UPrimaryAttributeSet::HandleManaAttribute(const FGSCAttributeSetExecutionData& ExecutionData)
{
    UGSCCoreComponent* TargetCoreComponent = ExecutionData.TargetCoreComponent;
    const float ClampMinimumValue = GetClampMinimumValueFor(GetManaAttribute());

    SetMana(FMath::Clamp(GetMana(), ClampMinimumValue, GetMaxMana()));

    if (TargetCoreComponent)
    {
        const float DeltaValue = ExecutionData.DeltaValue;
        const FGameplayTagContainer SourceTags = ExecutionData.SourceTags;
        TargetCoreComponent->HandleManaChange(DeltaValue, SourceTags);
    }
}