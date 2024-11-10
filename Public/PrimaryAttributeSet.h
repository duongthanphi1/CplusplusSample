// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Attributes/GSCAttributeSetBase.h"
#include "AbilitySystemComponent.h"
#include "PrimaryAttributeSet.generated.h"

UCLASS()
class RPGGAME_API UPrimaryAttributeSet : public UGSCAttributeSetBase
{
	GENERATED_BODY()

public:

	// Sets default values for this AttributeSet attributes
	UPrimaryAttributeSet();

    // AttributeSet Overrides
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
        
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health = 100.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, Health)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth = 100.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, MaxHealth)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana = 0.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, Mana)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana = 0.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, MaxMana)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina = 0.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, Stamina)    
    
    UPROPERTY(BlueprintReadOnly, Category = "", ReplicatedUsing = OnRep_MaxStamina)
    FGameplayAttributeData MaxStamina = 0.0;
    ATTRIBUTE_ACCESSORS(UPrimaryAttributeSet, MaxStamina)    

protected:
    
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	virtual void HandleHealthAttribute(const FGSCAttributeSetExecutionData& ExecutionData);
	virtual void HandleStaminaAttribute(const FGSCAttributeSetExecutionData& ExecutionData);
	virtual void HandleManaAttribute(const FGSCAttributeSetExecutionData& ExecutionData);
	
	
};
