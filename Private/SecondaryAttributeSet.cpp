// Fill out your copyright notice in the Description page of Project Settings.


#include "SecondaryAttributeSet.h"

#include "Net/UnrealNetwork.h"

// Sets default values
USecondaryAttributeSet::USecondaryAttributeSet()
{
	// Default constructor
}

void USecondaryAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // This is called whenever attributes change, so for max attributes we want to scale the current totals to match
    Super::PreAttributeChange(Attribute, NewValue);

    // Set adjust code here
    //
    // Example:
    //
    // If a Max value changes, adjust current to keep Current % of Current to Max
    //
    // if (Attribute == GetMaxHealthAttribute())
    // {
    //     AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
    // }
}

void USecondaryAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    FGSCAttributeSetExecutionData ExecutionData;
    GetExecutionDataFromMod(Data, ExecutionData);

    // Set clamping or handling or "meta" attributes here (like damages)

    // Example 1: Using helpers to handle each attribute in their own methods (See GSCAttributeSet.cpp)

    // if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    // {
    //     HandleHealthAttribute(ExecutionData);
    // }

    // Example 2: Basic example to clamp the value of an Health Attribute between 0 and another MaxHealth Attribute

    // if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    // {
    //     SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
    // }
}

void USecondaryAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        
    DOREPLIFETIME_CONDITION_NOTIFY(USecondaryAttributeSet, Speed, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(USecondaryAttributeSet, Strength, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(USecondaryAttributeSet, Magic, COND_None, REPNOTIFY_Always);
    
    DOREPLIFETIME_CONDITION_NOTIFY(USecondaryAttributeSet, Flexibility, COND_None, REPNOTIFY_Always);
}

void USecondaryAttributeSet::OnRep_Speed(const FGameplayAttributeData& OldSpeed)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(USecondaryAttributeSet, Speed, OldSpeed);
}

void USecondaryAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(USecondaryAttributeSet, Strength, OldStrength);
}

void USecondaryAttributeSet::OnRep_Magic(const FGameplayAttributeData& OldMagic)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(USecondaryAttributeSet, Magic, OldMagic);
}

void USecondaryAttributeSet::OnRep_Flexibility(const FGameplayAttributeData& OldFlexibility)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(USecondaryAttributeSet, Flexibility, OldFlexibility);
}
