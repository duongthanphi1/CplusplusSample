// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrowdVisualizationCustomDataProcessor.h"
#include "MassCrowdRepresentationSubsystem.h"
#include "MassRepresentationFragments.h"
#include "MassVisualizationComponent.h"
#include "CrowdVisualizationFragment.h"
#include "MassCrowdAnimationTypes.h"
#include "MassUpdateISMProcessor.h"
#include "MassCrowdUpdateISMVertexAnimationProcessor.h"
#include "GameFramework/PlayerController.h"
#include "MassLODFragments.h"

namespace UE::CitySampleCrowd
{
	int32 bAllowKeepISMExtraFrameBetweenISM = 1;
	FAutoConsoleVariableRef CVarAllowKeepISMExtraFrameBetweenISM(TEXT("CitySample.crowd.AllowKeepISMExtraFrameBetweenISM"), bAllowKeepISMExtraFrameBetweenISM, TEXT("Allow the frost crowd visulaization to keep previous ISM an extra frame when switching between ISM"), ECVF_Default);

	int32 bAllowKeepISMExtraFrameWhenSwitchingToActor = 0;
	FAutoConsoleVariableRef CVarAllowKeepISMExtraFrameWhenSwitchingToActor(TEXT("CitySample.crowd.AllowKeepISMExtraFrameWhenSwitchingToActor"), bAllowKeepISMExtraFrameWhenSwitchingToActor, TEXT("Allow the frost crowd visulaization to keep ISM an extra frame when switching to spanwed actor"), ECVF_Default);

}

UMassProcessor_CrowdVisualizationCustomData::UMassProcessor_CrowdVisualizationCustomData()
	: EntityQuery_Conditional(*this), LODSubsystem(nullptr)
{
	ExecutionFlags = (int32)(EProcessorExecutionFlags::Client | EProcessorExecutionFlags::Standalone);

	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Representation);

	// Requires animation system to update vertex animation data first
	

	bRequiresGameThreadExecution = true; // due to read-write access to FMassRepresentationSubsystemSharedFragment
}

void UMassProcessor_CrowdVisualizationCustomData::ConfigureQueries()
{
	EntityQuery_Conditional.AddRequirement<FCrowdAnimationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery_Conditional.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery_Conditional.AddRequirement<FMassViewerInfoFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery_Conditional.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery_Conditional.AddRequirement<FMassRepresentationLODFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery_Conditional.AddChunkRequirement<FMassVisualizationChunkFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery_Conditional.SetChunkFilter(&FMassVisualizationChunkFragment::AreAnyEntitiesVisibleInChunk);
	EntityQuery_Conditional.AddSharedRequirement<FMassRepresentationSubsystemSharedFragment>(EMassFragmentAccess::ReadWrite);
}

void UMassProcessor_CrowdVisualizationCustomData::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery_Conditional.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
		{
			UpdateCrowdCustomData(Context);
		});
}

void UMassProcessor_CrowdVisualizationCustomData::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);
	World = Owner.GetWorld();
	check(World);
	LODSubsystem = UWorld::GetSubsystem<UMassLODSubsystem>(World);
}

void UMassProcessor_CrowdVisualizationCustomData::UpdateCrowdCustomData(FMassExecutionContext& Context)
{
	UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetMutableSharedFragment<FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;
	check(RepresentationSubsystem);
	FMassInstancedStaticMeshInfoArrayView ISMInfos = RepresentationSubsystem->GetMutableInstancedStaticMeshInfos();
	
	const int32 NumEntities = Context.GetNumEntities();
	TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
	TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
	TConstArrayView<FMassViewerInfoFragment> LODInfoList = Context.GetFragmentView<FMassViewerInfoFragment>();
	TConstArrayView<FMassRepresentationLODFragment> RepresentationLODList = Context.GetFragmentView<FMassRepresentationLODFragment>();
	TArrayView<FCrowdAnimationFragment> AnimationDataList = Context.GetMutableFragmentView<FCrowdAnimationFragment>();

	// 0-3 are anim data
	for (int32 EntityIdx = 0; EntityIdx < NumEntities; EntityIdx++)
	{
		const FTransformFragment& TransformFragment = TransformList[EntityIdx];
		FMassRepresentationFragment& Representation = RepresentationList[EntityIdx];
		const FMassViewerInfoFragment& LODInfo = LODInfoList[EntityIdx];
		const FMassRepresentationLODFragment& RepresentationLOD = RepresentationLODList[EntityIdx];
		
		FCrowdAnimationFragment& AnimationData = AnimationDataList[EntityIdx];
		//UE_LOG(LogTemp,Error,TEXT(" anim state %d"),AnimationData.AnimationStateIndex);

		if (Representation.CurrentRepresentation == EMassRepresentationType::StaticMeshInstance || 
			// Keeping an extra frame of ISM when switching to actors as sometime the actor isn't loaded and will not be display on lower hand platform.
		    ( UE::CitySampleCrowd::bAllowKeepISMExtraFrameWhenSwitchingToActor && 
			  Representation.PrevRepresentation == EMassRepresentationType::StaticMeshInstance && 
			 (Representation.CurrentRepresentation == EMassRepresentationType::LowResSpawnedActor || Representation.CurrentRepresentation == EMassRepresentationType::HighResSpawnedActor)) )
		{
			// @todo find a better way to do this
			// Skip instances unseen by ExclusiveInstanceViewerIdx 
			//if (ExclusiveInstanceViewerIdx == INDEX_NONE || LODInfo.bIsVisibleByViewer[ExclusiveInstanceViewerIdx])
			
				FMassInstancedStaticMeshInfo& ISMInfo = ISMInfos[Representation.StaticMeshDescHandle.ToIndex()];

				const float PrevLODSignificance = UE::CitySampleCrowd::bAllowKeepISMExtraFrameBetweenISM ? Representation.PrevLODSignificance : -1.0f;

				// Update Transform
				UMassUpdateISMProcessor::UpdateISMTransform(Context.GetEntity(EntityIdx), ISMInfo, TransformFragment.GetTransform()
					, Representation.PrevTransform, RepresentationLOD.LODSignificance, PrevLODSignificance);

				// Custom data layout is 0-3 are anim data,
				// Need 4 floats of padding after anim data
				const int32 CustomDataPaddingAmount = 4;

				// Add Vertex animation custom floats
				UMassCrowdUpdateISMVertexAnimationProcessor::UpdateISMVertexAnimation(ISMInfo, AnimationData, RepresentationLOD.LODSignificance, PrevLODSignificance, CustomDataPaddingAmount);
			
				
			
		}
		Representation.PrevTransform = TransformFragment.GetTransform();
		Representation.PrevLODSignificance = RepresentationLOD.LODSignificance;
	}
}