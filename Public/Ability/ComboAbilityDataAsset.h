// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "ComboAbilityDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FComboAbilityMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> Ability;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Distancerequired = 0;

	FComboAbilityMapping() = default;
	
};

USTRUCT(BlueprintType)
struct FComboAbilityMappingWinza
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="ComboAbility")
	TArray<FComboAbilityMapping> ComboAbilitys;

	FComboAbilityMappingWinza() = default;
	
};
UCLASS(BlueprintType)
class RPGGAME_API UComboAbilityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UComboAbilityDataAsset() = default;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="ComboAbilityWinza")
	TArray<FComboAbilityMappingWinza> ComboAbilitySet;
};
