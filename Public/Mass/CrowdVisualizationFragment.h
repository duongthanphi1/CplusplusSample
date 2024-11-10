// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "MassEntityTypes.h"
#include "MassObserverProcessor.h"
#include "Mass/CrowdCharacterDefinition.h"

#include "CrowdVisualizationFragment.generated.h"

struct FMassEntityQuery;

class UAnimToTextureDataAsset;

USTRUCT()
struct RPGGAME_API FCitySampleCrowdVisualizationFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "aa")
	FCrowdVisualizationID VisualizationID;
	
};





UCLASS()
class RPGGAME_API UCitySampleCrowdVisualizationFragmentInitializer : public UMassObserverProcessor
{
	GENERATED_BODY()
	
public:
	UCitySampleCrowdVisualizationFragmentInitializer();	

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

protected:
	FMassEntityQuery EntityQuery;
	UAnimToTextureDataAsset* GetAnimToTextureDataAsset(TSoftObjectPtr<UAnimToTextureDataAsset> SoftPtr);
};