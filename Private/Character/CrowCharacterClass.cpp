// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CrowCharacterClass.h"

#include "MassCommonFragments.h"
#include "MassCrowdAnimationTypes.h"
#include "MassEntityManager.h"
#include "MassEntityTypes.h"
#include "MassMovementFragments.h"
#include "Algo/AllOf.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Mass/CrowdCharacterDefinition.h"
#include "Mass/CrowdVisualizationFragment.h"


// Sets default values
ACrowCharacterClass::ACrowCharacterClass()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACrowCharacterClass::OnConstruction(const FTransform& Transform)
{
	
}

void ACrowCharacterClass::Randomize()
{
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();

	RandomizeFromStream(RandomStream);	
}

void ACrowCharacterClass::RandomizeFromStream(const FRandomStream& RandomStream)
{
	if (!CrowdCharacterData)
    	{
    		return;
    	}
    	
    	// Need to convert to a set first
    	
    	CharacterOptions.Randomize(*CrowdCharacterData, RandomStream);
    	FCrowdCharacterDefinition CrowdCharacterDefinition;
    	CharacterOptions.GenerateCharacterDefinition(CrowdCharacterData, CrowdCharacterDefinition);
    	BuildCharacterFromDefinition(CrowdCharacterDefinition);
}

void ACrowCharacterClass::OnGetOrSpawn(FMassEntityManager* EntitySubsystem, const FMassEntityHandle MassAgent)
{
	if (EntitySubsystem && EntitySubsystem->IsEntityActive(MassAgent))
	{
		if (FMassVelocityFragment* MassVelocityFragment = EntitySubsystem->GetFragmentDataPtr<FMassVelocityFragment>(MassAgent))
		{
			if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
			{
				CharMovement->Velocity = MassVelocityFragment->Value;
			}
		}

		// Copy mass data relevant to animation to a persistent place, to have a correct first frame of anim
		/*if (FTransformFragment* TransformFragment = EntitySubsystem->GetFragmentDataPtr<FTransformFragment>(MassAgent))
		{
			SpawnAnimData.MassEntityTransform = TransformFragment->GetTransform();
		}

		if (FMassLookAtFragment* LookAtFragment = EntitySubsystem->GetFragmentDataPtr<FMassLookAtFragment>(MassAgent))
		{
			SpawnAnimData.LookAtDirection = LookAtFragment->Direction;
		}

		SpawnAnimData.FarLODAnimSequence = nullptr;
		SpawnAnimData.FarLODPlaybackStartTime = 0.0f;
		if (FCrowdAnimationFragment* AnimFragment = EntitySubsystem->GetFragmentDataPtr<FCrowdAnimationFragment>(MassAgent))
		{
			if (AnimFragment->AnimToTextureData.IsValid() && AnimFragment->AnimToTextureData->Animations.IsValidIndex(AnimFragment->AnimationStateIndex))
			{
				SpawnAnimData.FarLODAnimSequence = AnimFragment->AnimToTextureData->AnimSequences[AnimFragment->AnimationStateIndex].AnimSequence;
				if (SpawnAnimData.FarLODAnimSequence)
				{
					SpawnAnimData.FarLODAnimSequence = AnimFragment->AnimToTextureData->AnimSequences[AnimFragment->AnimationStateIndex].AnimSequence;

					if (UWorld* World = GetWorld())
					{
						const float GlobalTime = World->GetTimeSeconds();
						const float SequenceLength = SpawnAnimData.FarLODAnimSequence->GetPlayLength();
						SpawnAnimData.FarLODPlaybackStartTime = FMath::Fmod(AnimFragment->GlobalStartTime - GlobalTime, SequenceLength);

						if (SpawnAnimData.FarLODPlaybackStartTime < 0.0f)
						{
							SpawnAnimData.FarLODPlaybackStartTime += SequenceLength;
						}
					}
				}
			}
			SpawnAnimData.bSwappedThisFrame = AnimFragment->bSwappedThisFrame;
		}

		if (FMassRepresentationLODFragment* RepresentationFragment = EntitySubsystem->GetFragmentDataPtr<FMassRepresentationLODFragment>(MassAgent))
		{
			SpawnAnimData.Significance = RepresentationFragment->LODSignificance;
		}
		*/

		FCitySampleCrowdVisualizationFragment* CitySampleVisualizationFragment = EntitySubsystem->GetFragmentDataPtr<FCitySampleCrowdVisualizationFragment>(MassAgent);
		if (CitySampleVisualizationFragment)
		{
			BuildCharacterFromID(CitySampleVisualizationFragment->VisualizationID);
		}
	}
}

void ACrowCharacterClass::BuildCharacter()
{
	FCrowdCharacterDefinition CharacterDefinition;
	CharacterOptions.GenerateCharacterDefinition(CrowdCharacterData, CharacterDefinition);
	BuildCharacterFromDefinition(CharacterDefinition);
	
}

void ACrowCharacterClass::BuildCharacterFromDefinition(const FCrowdCharacterDefinition& InCharacterDefinition)
{
	const TArray<FSoftObjectPath> AssetsToLoad = InCharacterDefinition.GetSoftPathsToLoad();
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	//UE_LOG(LogTemp,Error, TEXT("Set mesh is %d"),CharacterOptions.SkeletonType);
	bool bAllAssetsLoaded = Algo::AllOf(AssetsToLoad, [](const FSoftObjectPath& AssetToLoad) {
		return (AssetToLoad.ResolveObject() != nullptr);
	});

	// Get a streaming handle and clear it if we're currently loading
	TSharedPtr<FStreamableHandle> BuildStreamingHandle = StreamingHandles.FindOrAdd(TEXT("Build"));
	if (BuildStreamingHandle.IsValid() && BuildStreamingHandle->IsActive())
	{
		BuildStreamingHandle->CancelHandle();
	}
	
	if (bAllAssetsLoaded)
	{
		BuildCharacterFromDefinition_Internal(InCharacterDefinition);
	}
	else if (bShouldAsyncLoad)
	{
		BuildStreamingHandle = StreamableManager.RequestAsyncLoad(AssetsToLoad, FStreamableDelegate::CreateUObject(this, &ThisClass::BuildCharacterFromDefinition_Internal, InCharacterDefinition));
	}
	else
	{
		BuildStreamingHandle = StreamableManager.RequestSyncLoad(AssetsToLoad);
		BuildCharacterFromDefinition_Internal(InCharacterDefinition);
	}
}

void ACrowCharacterClass::BuildCharacterFromID(const FCrowdVisualizationID& InVisualizationID)
{
	CharacterOptions = InVisualizationID.ToCharacterOptions();
	if (CrowdCharacterData)
	{
		CharacterOptions.GenerateCharacterDefinition(CrowdCharacterData, PrivateCharacterDefinition);
		BuildCharacterFromDefinition(PrivateCharacterDefinition);
	}
}

// Called when the game starts or when spawned
void ACrowCharacterClass::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACrowCharacterClass::BuildCharacterFromDefinition_Internal(const FCrowdCharacterDefinition InCharacterDefinition)
{
	PrivateCharacterDefinition = InCharacterDefinition;
	LoadAnimToTextureDataAssets(InCharacterDefinition);

	UpdateMeshes(InCharacterDefinition);
	
}

void ACrowCharacterClass::UpdateMeshes(const FCrowdCharacterDefinition& CharacterDefinition)
{
	if (bLoadingAnimToTextureDataAssets)
	{
		return;
	}
	TArray<TSoftObjectPtr<USkeletalMesh>> CharacterMeshes = {
		
		AnimToTextureDataAssets->SkeletalMesh 
	};
	USkeletalMeshComponent* MeshComponent = GetMesh();
	UpdateSkeletalMesh(MeshComponent, AnimToTextureDataAssets->SkeletalMesh );
	

}

void ACrowCharacterClass::UpdateSkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent,
	TSoftObjectPtr<USkeletalMesh> SoftSkeletalMeshPtr)
{
	USkeletalMesh* NewMesh = SoftSkeletalMeshPtr.LoadSynchronous();
	// If we are not forcing InitAnim to tick animation, we won't know if we're rendering yet on the first tick but we most likely are. Force refresh bones to not t-pose for a frame
	UWorld* World = SkeletalMeshComponent->GetWorld();
	//UE_LOG(LogTemp,Error, TEXT("Set mesh %s"), *NewMesh->GetName());
	if(SkeletalMeshComponent->bUseRefPoseOnInitAnim && World)
	{
		EVisibilityBasedAnimTickOption InitialVisibilityBasedAnimTickOption = SkeletalMeshComponent->VisibilityBasedAnimTickOption;
		SkeletalMeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
		SkeletalMeshComponent->SetSkeletalMesh(NewMesh);

		TWeakObjectPtr<USkeletalMeshComponent> WeakSkeletalMeshComponent = SkeletalMeshComponent;
		World->GetTimerManager().SetTimerForNextTick([WeakSkeletalMeshComponent, InitialVisibilityBasedAnimTickOption]()
			{
				if (USkeletalMeshComponent* StrongSkeletalMeshComponent = WeakSkeletalMeshComponent.Get())
				{
					// After the first tick, go back to the original setting
					StrongSkeletalMeshComponent->VisibilityBasedAnimTickOption = InitialVisibilityBasedAnimTickOption;
				}
			});
	}
	else
	{
		SkeletalMeshComponent->SetSkeletalMesh(NewMesh);
	}
}

void ACrowCharacterClass::LoadAnimToTextureDataAssets(const FCrowdCharacterDefinition& InCharacterDefinition)
{
	bLoadingAnimToTextureDataAssets = false;

	
		if (AnimToTextureDataAssetsStreamingHandles && AnimToTextureDataAssetsStreamingHandles->IsActive())
		{
			AnimToTextureDataAssetsStreamingHandles->CancelHandle();
		}
		AnimToTextureDataAssets = nullptr;
	

	AsyncLoadAnimToTextureDataAsset(InCharacterDefinition.CharacterData);
}

void ACrowCharacterClass::AsyncLoadAnimToTextureDataAsset(TSoftObjectPtr<UAnimToTextureDataAsset> Asset)
{
	if (!Asset.IsValid())
	{
		if (!Asset.IsNull())
		{
			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			StreamableManager.RequestAsyncLoad(Asset.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::AnimToTextureDataAssetLoaded, Asset));
			bLoadingAnimToTextureDataAssets = true;
		}
	}
	else
	{
		// Asset is already in memory
		AnimToTextureDataAssets = Asset.Get();
	}
}

void ACrowCharacterClass::AnimToTextureDataAssetLoaded(TSoftObjectPtr<UAnimToTextureDataAsset> Asset)
{
	AnimToTextureDataAssets = Asset.Get();
    
    	
	if (!AnimToTextureDataAssets)
	{
		return;
	}
    	
    
	bLoadingAnimToTextureDataAssets = false;
    
	UpdateMeshes(PrivateCharacterDefinition);
}

// Called every frame
void ACrowCharacterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACrowCharacterClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

