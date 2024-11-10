// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/MassCrownInteractionSubsystem.h"

#include "MassAgentComponent.h"
#include "MassAgentSubsystem.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeTypes.h"
#include "Core/MassInteractionComponent.h"




const FMassInteractionType* UMassCrownInteractionSubsystem::LastInteracActor(const FMassEntityHandle Entity) const
{
	return InteractResults.Find(Entity);
}

void UMassCrownInteractionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AgentSubsystem = Collection.InitializeDependency<UMassAgentSubsystem>();
	checkfSlow(AgentSubsystem != nullptr, TEXT("MassAgentSubsystem is required"));

	SignalSubsystem = Collection.InitializeDependency<UMassSignalSubsystem>();
	

	AgentSubsystem->GetOnMassAgentComponentEntityAssociated().AddLambda([this](const UMassAgentComponent& AgentComponent)
	{
		if (UMassInteractionComponent* InteractionComponent = AgentComponent.GetOwner()->FindComponentByClass<UMassInteractionComponent>())
		{
			RegisterForInteration(AgentComponent.GetEntityHandle(), *InteractionComponent);
			
		}
	});

	AgentSubsystem->GetOnMassAgentComponentEntityDetaching().AddLambda([this](const UMassAgentComponent& AgentComponent)
	{
		if (UMassInteractionComponent* InteractionComponent = AgentComponent.GetOwner()->FindComponentByClass<UMassInteractionComponent>())
		{
			UnregisterForInteration(AgentComponent.GetEntityHandle(), *InteractionComponent);
			
		}
	});
}

void UMassCrownInteractionSubsystem::Deinitialize()
{
	checkfSlow(AgentSubsystem != nullptr, TEXT("MassAgentSubsystem must have be set during initialization"));
	AgentSubsystem->GetOnMassAgentComponentEntityAssociated().RemoveAll(this);
	AgentSubsystem->GetOnMassAgentComponentEntityDetaching().RemoveAll(this);
}

void UMassCrownInteractionSubsystem::Tick(float DeltaTime)
{
	const UWorld* World = GetWorld();
	check(World);

	const double CurrentTime = World->GetTimeSeconds();
	constexpr double HitResultDecayDuration = 1.;
	
	for (auto Iter = InteractResults.CreateIterator(); Iter; ++Iter)
	{
		const FMassInteractionType& Damaged = Iter.Value();
		const double ElapsedTime = CurrentTime - Damaged.LastFilteredInteractTime;
		FMassEntityHandle a= Iter.Key();

		//UE_LOG(LogTemp,Error,TEXT("Got Damage in Entity %d"),a.Index);
		if (ElapsedTime > HitResultDecayDuration)
		{
			/*FMassEntityHandle a= Iter.Key();
			UE_LOG(LogTemp,Error,TEXT("Remove Current Entity %d with elapsedtime : %f from damage time %f in %f"),a.Index, ElapsedTime,Damaged.LastFilteredHitTime,
				CurrentTime);*/
			Iter.RemoveCurrent();
		}
	}
}

TStatId UMassCrownInteractionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMassInteractionComponent, STATGROUP_Tickables);
}

void UMassCrownInteractionSubsystem::RegisterForInteration(const FMassEntityHandle Entity,
	UMassInteractionComponent& CoreComponent)
{
	EntityToComponentMap.Add(Entity,CoreComponent);
	ComponentToEntityMap.Add(CoreComponent,Entity);
	CoreComponent.OnBumped.AddDynamic(this,&UMassCrownInteractionSubsystem::OnBumpCallBack);
	//UE_LOG(LogTemp,Warning,TEXT("S1 %d "),ComponentToEntityMap.Num());
}

void UMassCrownInteractionSubsystem::UnregisterForInteration(FMassEntityHandle Entity,
	UMassInteractionComponent& CoreComponent)
{
	EntityToComponentMap.Remove(Entity);
	ComponentToEntityMap.Remove(&CoreComponent);
	CoreComponent.OnBumped.RemoveAll(this);
}

void UMassCrownInteractionSubsystem::OnBumpCallBack(UMassInteractionComponent* OtherComponent,UMassInteractionComponent* PlayerComponent)
{
	const UWorld* World = GetWorld();
	check(World);
	//UE_LOG(LogTemp,Error,TEXT("IInteracCall"));
	
	const FMassEntityHandle Entity = ComponentToEntityMap.FindChecked(OtherComponent);
	FMassEntityHandle* PlayerEntity = ComponentToEntityMap.Find(PlayerComponent);
	const double CurrentTime = World->GetTimeSeconds();

	//double HitResultMergeDuration = 1.;
	FMassInteractionType* ExitsResult= InteractResults.Find(Entity);
	bool bProcessHit = true;
	if (ExitsResult)
	{
		ExitsResult->LastFilteredInteractTime = CurrentTime;
		bProcessHit = false;
	}
	
	if (bProcessHit)
	{
		InteractResults.Add(Entity,{*PlayerEntity,CurrentTime});
	}
	SignalSubsystem->SignalEntity(UE::Mass::Signals::CrownInteract,Entity);
	
}
