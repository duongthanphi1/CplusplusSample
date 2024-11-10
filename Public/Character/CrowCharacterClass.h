// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimToTextureDataAsset.h"
#include "IMassCrowdActor.h"
#include "Mass/CharacterCrownDataAsset.h"
#include "Mass/CrowdCharacterDefinition.h"
#include "ModularGameplayActors/GSCModularCharacter.h"

#include "CrowCharacterClass.generated.h"

struct FStreamableHandle;
class UStaticMeshComponent;
class UMassAgentComponent;



UCLASS(DefaultToInstanced)
class RPGGAME_API ACrowCharacterClass : public AGSCModularCharacter, public IMassCrowdActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACrowCharacterClass();
	// Uses the currently set options for the actor to build the corresponding character
	UFUNCTION(BlueprintCallable, Category = "Crowd")
	void BuildCharacter();

	UFUNCTION(BlueprintCallable, Category = "Crowd")
	void BuildCharacterFromDefinition(const FCrowdCharacterDefinition& InCharacterDefinition);

	UFUNCTION(BlueprintCallable, Category = "Crowd")
	void BuildCharacterFromID(const FCrowdVisualizationID& InVisualizationID);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Crowd")
	TObjectPtr<UCharacterCrownDataAsset> CrowdCharacterData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character", Interp, meta = (EditCondition="CharacterDataType==ECharacterDataType::Crowd", EditConditionHides))
	FCrowdCharacterOptions CharacterOptions;

	TMap<FString, TSharedPtr<FStreamableHandle>> StreamingHandles;

	// Whether to asynchronously load any assets which have not previously been loaded
	UPROPERTY(EditAnywhere, Category="Character", Interp)
	bool bShouldAsyncLoad = true;

	UPROPERTY()
	FCrowdCharacterDefinition PrivateCharacterDefinition;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Character")
	void Randomize();

	UFUNCTION(BlueprintCallable, CallInEditor, Category="Character")
	void RandomizeFromStream(const FRandomStream& RandomStream);

	virtual void OnGetOrSpawn(FMassEntityManager* EntitySubsystem, const FMassEntityHandle MassAgent) override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

private:
	void BuildCharacterFromDefinition_Internal(const FCrowdCharacterDefinition InCharacterDefinition);
	void UpdateMeshes(const FCrowdCharacterDefinition& CharacterDefinition);

	void UpdateSkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent, TSoftObjectPtr<USkeletalMesh> SoftSkeletalMeshPtr);

	TArray<TSoftObjectPtr<USkeletalMesh>> StreamingMeshes;

	void LoadAnimToTextureDataAssets(const FCrowdCharacterDefinition& InCharacterDefinition);
	void AsyncLoadAnimToTextureDataAsset(TSoftObjectPtr<UAnimToTextureDataAsset> Asset);
	void AnimToTextureDataAssetLoaded(TSoftObjectPtr<UAnimToTextureDataAsset> Asset);
	bool bLoadingAnimToTextureDataAssets = false;
	bool bGroomComponentAttachmentDelayed = false;

	UPROPERTY(Transient)
	UAnimToTextureDataAsset* AnimToTextureDataAssets;

	TSharedPtr<FStreamableHandle> AnimToTextureDataAssetsStreamingHandles;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
