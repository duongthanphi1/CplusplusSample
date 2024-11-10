// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimToTextureDataAsset.h"
#include "Engine/DataAsset.h"
#include "CharacterCrownDataAsset.generated.h"


USTRUCT(BlueprintType)
struct FCharacterDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client"))
	UAnimToTextureDataAsset* AnimToTextureDataAsset;
};


/**
 * 
 */
UCLASS()
class RPGGAME_API UCharacterCrownDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, meta= (AssetBundles = "client"))
	TArray<FCharacterDefinition> CharacterDefinition;

	UFUNCTION()
	UAnimToTextureDataAsset* GetAnimtoTextureDataAsset(TArray<FCharacterDefinition> CharacterDefinitions);
	
};

