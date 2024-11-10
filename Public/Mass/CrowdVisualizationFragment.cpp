// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdVisualizationFragment.h"
#include "MassCrowdAnimationTypes.h"
#include "MassExecutionContext.h"
#include "MassCrowdRepresentationSubsystem.h"
#include "MassRepresentationTypes.h"
#include "MassRepresentationFragments.h"
#include "MassCrowdAnimationTypes.h"
#include "CitySampleCrowdSettings.h"
#include "Character/CrowCharacterClass.h"
#include "AnimToTexture/Public/AnimToTextureDataAsset.h"
//#include "Engine/SkinnedAssetCommon.h"

static TAutoConsoleVariable<bool> CVarUseMetahumanPresets(
    TEXT("Crowd.UseMetahumanPresets"),
    false,
    TEXT("Controls whether the Visualization fragment will attempt to use a data asset of presets to control the randomization"),
	ECVF_Cheat);

static TAutoConsoleVariable<FString> CVarMetahumanPresetsPath(
    TEXT("Crowd.MetahumanPresetsPath"),
    "/Game/Crowd/Character/Shared/CrowdPresets.CrowdPresets",
    TEXT("Path to the data asset containing the Metahuman Presets"),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarUseISMFarLod(
	TEXT("Crowd.UseISMFarLOD"),
	true,
	TEXT("Controls whether we'll use an alternate user-defined (in CrowdCharacterDataAsset) simplified ISM for furthest crowds"),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarUseISMFarLodMeshOverride(
	TEXT("Crowd.UseISMFarLodMeshOverride"),
	false,
	TEXT("Controls whether we'll use the static mesh override for Far LODs defined in UCitySampleCrowdSettings"),
	ECVF_Cheat);

static TAutoConsoleVariable<bool> CVarUseMetahumanISMParts(
	TEXT("Crowd.UseMetahumanISMParts"),
	true,
	TEXT("Controls whether the Visualization fragment will use parts"),
	ECVF_Cheat);

static TAutoConsoleVariable<int> CVarMaxCrowdHeads(
	TEXT("Crowd.MaxHeads"),
	0,
	TEXT("Set the maximum number of heads to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdOutfits(
	TEXT("Crowd.MaxOutfits"),
	0,
	TEXT("Set the maximum number of outfits to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdOutfitMaterials(
	TEXT("Crowd.MaxOutfitMaterials"),
	0,
	TEXT("Set the maximum number of outfit materials to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdHairstyles(
	TEXT("Crowd.MaxHairstyles"),
	0,
	TEXT("Set the maximum number of hairstyles to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdEyebrows(
	TEXT("Crowd.MaxEyebrows"),
	0,
	TEXT("Set the maximum number of eyebrows to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdEyelashes(
	TEXT("Crowd.MaxEyelashes"),
	0,
	TEXT("Set the maximum number of eyelashes to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdMustaches(
	TEXT("Crowd.MaxMustaches"),
	0,
	TEXT("Set the maximum number of mustaches to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdBeards(
	TEXT("Crowd.MaxBeards"),
	0,
	TEXT("Set the maximum number of beards to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdHairColors(
	TEXT("Crowd.MaxHairColors"),
	0,
	TEXT("Set the maximum number of hair colors to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdSkinTextures(
	TEXT("Crowd.MaxSkinTextures"),
	0,
	TEXT("Set the maximum number of skin textures to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdSkinTextureModifiers(
	TEXT("Crowd.MaxSkinTextureModifiers"),
	0,
	TEXT("Set the maximum number of skin texture modifiers to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarMaxCrowdAccessories(
	TEXT("Crowd.MaxAccessories"),
	0,
	TEXT("Set the maximum number of accessories to use for crowds. Setting this to 0 or lower will be unlimited"),
	ECVF_Scalability);

static TAutoConsoleVariable<int> CVarCrowdRandomSeed(
	TEXT("Crowd.RandomSeed"),
	376789,
	TEXT("Set's a random seed to ensure crowds are generated in a consistent way for meaningful performance comparisons"),
	ECVF_Cheat);

UCitySampleCrowdVisualizationFragmentInitializer::UCitySampleCrowdVisualizationFragmentInitializer()
	: EntityQuery(*this)
{
	ObservedType = FMassMontageFragment::StaticStruct();
	Operation = EMassObservedOperation::Add;
}

void UCitySampleCrowdVisualizationFragmentInitializer::ConfigureQueries() 
{
	EntityQuery.AddRequirement<FCitySampleCrowdVisualizationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FCrowdAnimationFragment>(EMassFragmentAccess::ReadWrite);
}

void UCitySampleCrowdVisualizationFragmentInitializer::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	UMassCrowdRepresentationSubsystem* RepresentationSubsystem = UWorld::GetSubsystem<UMassCrowdRepresentationSubsystem>(EntityManager.GetWorld());
	
	// Build the Random Stream from the seed if one is provided
	FRandomStream RandomStream;
    
    	const int32 CrowdRandomSeed = CVarCrowdRandomSeed.GetValueOnGameThread();
    	if (CrowdRandomSeed >= 0)
    	{
    		RandomStream.Initialize(CrowdRandomSeed);
    	}
    	else
    	{
    		RandomStream.GenerateNewSeed();
    	}
	// Fallback random if we were unable to set up using the presets
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this, RepresentationSubsystem, RandomStream](FMassExecutionContext& Context)
	{
		const TArrayView<FCitySampleCrowdVisualizationFragment> VisualizationList = Context.GetMutableFragmentView<FCitySampleCrowdVisualizationFragment>();
		const TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
		//const TArrayView<FCrowdAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimationFragment>();
		
		const int32 NumEntities = Context.GetNumEntities();
		for (int32 i = 0; i < NumEntities; ++i)
		{
			//UE_LOG(LogTemp,Error,TEXT("ABC"));
			// Try to get the data asset from the crowd character CDO
			ACrowCharacterClass* CrowCharacterCDO = nullptr;
			UCharacterCrownDataAsset* CrowdCharacterDataAsset = nullptr;
			TSubclassOf<AActor> TemplateActorClass = RepresentationSubsystem->GetTemplateActorClass(RepresentationList[i].HighResTemplateActorIndex);
			if (TemplateActorClass)
			{
				CrowCharacterCDO = Cast<ACrowCharacterClass>(TemplateActorClass->GetDefaultObject());
				if (CrowCharacterCDO)
				{
					CrowdCharacterDataAsset = CrowCharacterCDO->CrowdCharacterData.Get();
					
				}
			}
			
			// Grab the settings class
			const UCitySampleCrowdSettings* CrowdSettings = UCitySampleCrowdSettings::Get();

			// Randomize using the data asset if possible else fallback to a generic randomize
			FCrowdCharacterOptions CharacterOptions;
            if (CrowdSettings && CrowdSettings->bMassCrowdShouldUseActorDefaultOptions && CrowCharacterCDO)
            {
            	CharacterOptions = CrowCharacterCDO->CharacterOptions;
            	VisualizationList[i].VisualizationID = FCrowdVisualizationID(CharacterOptions);
            }
            else if (CrowdCharacterDataAsset)
            	{
            		CharacterOptions.Randomize(*CrowdCharacterDataAsset, RandomStream);
            				
            		VisualizationList[i].VisualizationID = FCrowdVisualizationID(CharacterOptions);
            	}
            else
            	{
            		VisualizationList[i].VisualizationID.Randomize(RandomStream);
            	}

			if ( RepresentationSubsystem)
			{
				if (CrowdCharacterDataAsset)
				{
					QUICK_SCOPE_CYCLE_COUNTER(STAT_CrowdVisualizationFragmentInitializer_ISMSetup);
					
					FCrowdCharacterDefinition CharacterDefinition;

					

					// If we found the data asset then we should have a valid set of Character Options already
					// from when we randomized
					CharacterOptions.GenerateCharacterDefinition(CrowdCharacterDataAsset, CharacterDefinition);

					FStaticMeshInstanceVisualizationDesc StaticMeshInstanceDesc;
					StaticMeshInstanceDesc.bUseTransformOffset = true;
					StaticMeshInstanceDesc.TransformOffset.SetRotation(FRotator(0, -90.0f, 0).Quaternion());

					const bool bUseFarLod = CVarUseISMFarLod.GetValueOnGameThread() == true;
					const float FarLodSignificanceThreshold = CrowdSettings ? CrowdSettings->ISMFarLodSignificanceThreshold : 4.0f;

					FMassStaticMeshInstanceVisualizationMeshDesc StaticMeshDesc;
					StaticMeshDesc.MinLODSignificance = 0.0f;
					StaticMeshDesc.MaxLODSignificance = bUseFarLod ? FarLodSignificanceThreshold : 4.0f;
					if (UAnimToTextureDataAsset* ATTDA = GetAnimToTextureDataAsset(CharacterDefinition.CharacterData))
					{
						ensureMsgf(ATTDA->GetStaticMesh(), TEXT("%s is missing static mesh %s"), *ATTDA->GetName(), *ATTDA->StaticMesh.ToString());

						StaticMeshDesc.Mesh = ATTDA->GetStaticMesh();
						StaticMeshInstanceDesc.Meshes.Add(StaticMeshDesc);
					}

					

					/*if (false)
					{
						//FCrowdGenderDefinition& GenderDefinition = CharacterOptions.Skeleton == ECitySampleCrowdGender::A ? CrowdCharacterDataAsset->SkeletonA : CrowdCharacterDataAsset->SkeletonB;
						if(AnimToTextureDataAsset)
						{
							const bool bUseFarLodMeshOverride = CVarUseISMFarLodMeshOverride.GetValueOnGameThread() == true;

							StaticMeshDesc.MinLODSignificance = FarLodSignificanceThreshold;
							StaticMeshDesc.MaxLODSignificance = 4.0f;
							
							UStaticMesh* MeshOverride = (bUseFarLodMeshOverride && CrowdSettings) ? CrowdSettings->GetISMFarLodMeshOverride() : nullptr;
							if (MeshOverride)
							{
								StaticMeshDesc.Mesh = MeshOverride;
							}
							else
							{
								StaticMeshDesc.Mesh = AnimToTextureDataAsset->GetStaticMesh();
							}

							StaticMeshInstanceDesc.Meshes.Add(StaticMeshDesc);
						}
					}*/

					RepresentationList[i].StaticMeshDescHandle = RepresentationSubsystem->FindOrAddStaticMeshDesc(StaticMeshInstanceDesc);
				}
			}



			if (CrowdSettings)
			{
				if (CrowdSettings->bForceMassCrowdToAverage)
				{
					// Force body type to average for alpha
					//VisualizationList[i].VisualizationID.AsMutableBitfield().BodyType = 0;
				}

				if (CrowdSettings->bHideAccessoriesForMassCrowd)
				{
					// Force off accessories for alpha
					//VisualizationList[i].VisualizationID.AsMutableBitfield().AccessoryIndex = 0;
				}
			}
		}
	});

};


UAnimToTextureDataAsset* UCitySampleCrowdVisualizationFragmentInitializer::GetAnimToTextureDataAsset(TSoftObjectPtr<UAnimToTextureDataAsset> SoftPtr)
{
	if (SoftPtr.IsNull())
	{
		return nullptr;
	}

	if (SoftPtr.IsValid())
	{ 
		return SoftPtr.Get();
	}

	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_CitySampleCrowdVisualizationFragmentInitializer_GetAnimToTextureDataAsset_LoadSync);
		return SoftPtr.LoadSynchronous();
	}
}