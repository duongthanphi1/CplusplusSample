// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "GamePlayMassageForUI.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(DamageMessage);
/**
 * 
 */
UCLASS()
class RPGGAME_API UGamePlayMassageForUI : public UObject
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FDamageMassage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FHitResult ActorHit;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer Tags;
};
