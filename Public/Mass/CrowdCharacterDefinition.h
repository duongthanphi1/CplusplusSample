// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AnimToTextureDataAsset.h"
#include "Containers/Map.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Math/Color.h"
#include "UObject/SoftObjectPtr.h"
#include "Mass/CharacterCrownDataAsset.h"

#include "CrowdCharacterDefinition.generated.h"

//class UCrowdCharacterDataAsset;



USTRUCT(BlueprintType)
struct RPGGAME_API FCrowdCharacterDefinition
{
	GENERATED_BODY()

	FCrowdCharacterDefinition()
	{
		
	}


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimToTextureDataAsset> CharacterData;

	TArray<FSoftObjectPath> GetSoftPathsToLoad() const;
};

USTRUCT(BlueprintType)
struct RPGGAME_API FCrowdCharacterOptions
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lineup|Base")
	uint8 SkeletonType = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lineup|Base", Interp)
	uint8 AnimSetIndex = 0;


	void GenerateCharacterDefinition(const UCharacterCrownDataAsset* DataAsset, FCrowdCharacterDefinition& CharacterDefinition) const;
	
	// Use the provided data asset to generate a random set of valid options
	//void Randomize(const UCharacterCrownDataAsset& DataAsset);
	void Randomize(const UCharacterCrownDataAsset& DataAsset, const FRandomStream& RandomStream);
	/*void Randomize(const UCrowdCharacterDataAsset& DataAsset, TSet<ECrowdLineupVariation> FixedProperties);
	void Randomize(const UCrowdCharacterDataAsset& DataAsset, TSet<ECrowdLineupVariation> FixedProperties, const FRandomStream& RandomStream);*/

	bool operator==(const FCrowdCharacterOptions& OtherOptions) const
	{
		return
				SkeletonType				==	OtherOptions.SkeletonType
				
				&&AnimSetIndex				== OtherOptions.AnimSetIndex;
	}

	bool operator!=(const FCrowdCharacterOptions& OtherOptions) const
	{
		return !(*this == OtherOptions);
	}
};

// This struct represents the packed character options for use with FCrowdVisualizationID
// The actual struct will be stored as an int64 in the VisualizationID to work around
// the property system not supporting bitfields
struct FCrowdVisualizationBitfield
{
	uint64 Skeleton					: 1;
	uint64 AnimSetIndex				: 3;
};

USTRUCT(BlueprintType)
struct FCrowdVisualizationID
{
	GENERATED_BODY()

	// All the data is packed inside of an int64 and can be accessed by reinterpreting as
	// a FCrowdVisualizationBitfield. See FCrowdVisualizationID::AsMutableBitfield.
	UPROPERTY()
	int64 PackedData = 0;

	FCrowdVisualizationID() = default;
	FCrowdVisualizationID(const FCrowdCharacterOptions InOptions);

	// Convert the ID to a full options struct
	FCrowdCharacterOptions ToCharacterOptions() const;

	// Reinterprets the packed data as a bitfield struct for easier use
	// provides both a mutable and an immutable version;
	FCrowdVisualizationBitfield& AsMutableBitfield();
	const FCrowdVisualizationBitfield& AsBitfield() const;

	// Randomization Functions
	void Randomize();
	void Randomize(int32 InSeed);
	void Randomize(const FRandomStream& RandomStream);

};





