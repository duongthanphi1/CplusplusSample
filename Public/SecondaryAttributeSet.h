// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Attributes/GSCAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "SecondaryAttributeSet.generated.h"

UCLASS()
class RPGGAME_API USecondaryAttributeSet : public UGSCAttributeSetBase
{
	GENERATED_BODY()

public:

	// Sets default values for this AttributeSet attributes
	USecondaryAttributeSet();

    // AttributeSet Overrides
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
        
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Speed)
    FGameplayAttributeData Speed = 360.0;
    ATTRIBUTE_ACCESSORS(USecondaryAttributeSet, Speed)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Strength)
    FGameplayAttributeData Strength = 1.0;
    ATTRIBUTE_ACCESSORS(USecondaryAttributeSet, Strength)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Magic)
    FGameplayAttributeData Magic = 1.0;
    ATTRIBUTE_ACCESSORS(USecondaryAttributeSet, Magic)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Flexibility)
    FGameplayAttributeData Flexibility = 1.0;
    ATTRIBUTE_ACCESSORS(USecondaryAttributeSet, Flexibility)    

protected:
    
    UFUNCTION()
    virtual void OnRep_Speed(const FGameplayAttributeData& OldSpeed);

    UFUNCTION()
    virtual void OnRep_Strength(const FGameplayAttributeData& OldStrength);

    UFUNCTION()
    virtual void OnRep_Magic(const FGameplayAttributeData& OldMagic);

    UFUNCTION()
    virtual void OnRep_Flexibility(const FGameplayAttributeData& OldFlexibility);
	
	
};
