// Copyright Epic Games, Inc. All Rights Reserved.

#include "Mass/CrowdCharacterDefinition.h"

#include "Engine/AssetManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/RandomStream.h"



//DEFINE_LOG_CATEGORY(LogCitySampleCrowdDefinition);





TArray<FSoftObjectPath> FCrowdCharacterDefinition::GetSoftPathsToLoad() const
{
	// Initialize with the direct SoftObjectPtrs
	TArray<FSoftObjectPath> ObjectsToLoad = {
		
	};

	// Add all the hair definitions
	

	TArray<TSoftObjectPtr<UAnimToTextureDataAsset>> AnimToTextureDataAssets = {
		CharacterData
	};

	for (TSoftObjectPtr<UAnimToTextureDataAsset> SoftAnimToTextureDataAsset : AnimToTextureDataAssets)
	{
		// Assume all the AnimToTextureDataAssets are already loaded
		if (UAnimToTextureDataAsset* AnimToTextureDataAsset = SoftAnimToTextureDataAsset.LoadSynchronous())
		{
			ObjectsToLoad.Add(AnimToTextureDataAsset->SkeletalMesh.ToSoftObjectPath());
		}
	}

	/*// Animation data assets:
	if (!LocomotionAnimSet.IsNull())
	{
		ObjectsToLoad.Add(LocomotionAnimSet.ToSoftObjectPath());
	}

	if (!ContextualAnimDataAsset.IsNull())
	{
		ObjectsToLoad.Add(ContextualAnimDataAsset.ToSoftObjectPath());
	}*/

	// Strip any null assets from the list
	int i = 0;
	while (i < ObjectsToLoad.Num())
	{
		if (ObjectsToLoad[i].IsNull())
		{
			ObjectsToLoad.RemoveAt(i);
		}
		else
		{
			i += 1;
		}
	}

	return ObjectsToLoad;
}



/*void FCrowdCharacterOptions::Randomize(const UCharacterCrownDataAsset& DataAsset)
{
	SkeletonType = FMath::RandRange(0,DataAsset.CharacterDefinition.Num());
	
}*/

void FCrowdCharacterOptions::GenerateCharacterDefinition(const UCharacterCrownDataAsset* DataAsset,
                                                         FCrowdCharacterDefinition& CharacterDefinition) const
{
	if (!DataAsset)
	{
		return;
	}

	// Reset the Definition to prevent any state carrying over
	CharacterDefinition = FCrowdCharacterDefinition();

	CharacterDefinition.CharacterData = DataAsset->CharacterDefinition[SkeletonType].AnimToTextureDataAsset;
}

void FCrowdCharacterOptions::Randomize(const UCharacterCrownDataAsset& DataAsset, const FRandomStream& RandomStream)
{
	SkeletonType = RandomStream.RandRange(0,DataAsset.CharacterDefinition.Num()-1);
	//UE_LOG(LogTemp,Error, TEXT("Random SkeletonType is %d"),SkeletonType);
}


FCrowdVisualizationID::FCrowdVisualizationID(const FCrowdCharacterOptions InOptions)
{
	FCrowdVisualizationBitfield& VisualizationBitfield = AsMutableBitfield();

	VisualizationBitfield.Skeleton					= static_cast<uint8>(InOptions.SkeletonType);
	
	VisualizationBitfield.AnimSetIndex				= InOptions.AnimSetIndex;
}

FCrowdCharacterOptions FCrowdVisualizationID::ToCharacterOptions() const
{
	FCrowdCharacterOptions OutOptions;

	const FCrowdVisualizationBitfield& VisualizationBitfield = AsBitfield();

	
	OutOptions.SkeletonType				= VisualizationBitfield.Skeleton;
	OutOptions.AnimSetIndex				= VisualizationBitfield.AnimSetIndex;

	return OutOptions;
}

FCrowdVisualizationBitfield& FCrowdVisualizationID::AsMutableBitfield()
{
	// Ensure the bitfield struct we're using fits inside an int64
	static_assert(sizeof(FCrowdVisualizationBitfield) <= sizeof(int64));
	
	return reinterpret_cast<FCrowdVisualizationBitfield&>(this->PackedData);
}

const FCrowdVisualizationBitfield& FCrowdVisualizationID::AsBitfield() const
{
	// Ensure the bitfield struct we're using fits inside an int64
	static_assert(sizeof(FCrowdVisualizationBitfield) <= sizeof(int64));
	
	return reinterpret_cast<const FCrowdVisualizationBitfield&>(this->PackedData);
}

void FCrowdVisualizationID::Randomize()
{
	FRandomStream RandomStream;
	RandomStream.GenerateNewSeed();
	Randomize(RandomStream);
}

void FCrowdVisualizationID::Randomize(int32 InSeed)
{
	FRandomStream RandomStream;
	RandomStream.Initialize(InSeed);
	Randomize(RandomStream);
}

void FCrowdVisualizationID::Randomize(const FRandomStream& RandomStream)
{
	// RandomStream only generates uint32's so we'll just compose two
	// random uint32s to make a random int64
	uint32* PackedDataAddress = reinterpret_cast<uint32*>(&PackedData);
	*PackedDataAddress	= RandomStream.GetUnsignedInt();
	*(PackedDataAddress + 1) = RandomStream.GetUnsignedInt();
	
}