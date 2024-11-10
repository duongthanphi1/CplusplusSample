// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/MassDamageSubsystem.h"

#include "MassActorSubsystem.h"
#include "MassAgentComponent.h"
#include "MassAgentSubsystem.h"
#include "MassStateTreeTypes.h"
#include "MotionWarpingComponent.h"
#include "Components/GSCCoreComponent.h"


const FMassDamageType* UMassDamageSubsystem::LastDamageActor(const FMassEntityHandle Entity) const
{
	return DamageResult.Find(Entity);
}

TStatId UMassDamageSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UMassDamageSubsystem, STATGROUP_Tickables);
}

void UMassDamageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AgentSubsystem = Collection.InitializeDependency<UMassAgentSubsystem>();
	checkfSlow(AgentSubsystem != nullptr, TEXT("MassAgentSubsystem is required"));

	SignalSubsystem = Collection.InitializeDependency<UMassSignalSubsystem>();

	AgentSubsystem->GetOnMassAgentComponentEntityAssociated().AddLambda([this](const UMassAgentComponent& AgentComponent)
	{
		if (UGSCCoreComponent* CoreComponent = AgentComponent.GetOwner()->FindComponentByClass<UGSCCoreComponent>())
		{
			RegisterForHealthChange(AgentComponent.GetEntityHandle(), *CoreComponent);
			
		}
	});

	AgentSubsystem->GetOnMassAgentComponentEntityDetaching().AddLambda([this](const UMassAgentComponent& AgentComponent)
	{
		if (UGSCCoreComponent* CoreComponent = AgentComponent.GetOwner()->FindComponentByClass<UGSCCoreComponent>())
		{
			UnregisterForHealthChange(AgentComponent.GetEntityHandle(), *CoreComponent);
		}
	});
}

void UMassDamageSubsystem::Deinitialize()
{
	checkfSlow(AgentSubsystem != nullptr, TEXT("MassAgentSubsystem must have be set during initialization"));
	AgentSubsystem->GetOnMassAgentComponentEntityAssociated().RemoveAll(this);
	AgentSubsystem->GetOnMassAgentComponentEntityDetaching().RemoveAll(this);
}

void UMassDamageSubsystem::Tick(float DeltaTime)
{
	const UWorld* World = GetWorld();
	check(World);

	const double CurrentTime = World->GetTimeSeconds();
	constexpr double HitResultDecayDuration = 1.;
	
	for (auto Iter = DamageResult.CreateIterator(); Iter; ++Iter)
	{
		const FMassDamageType& Damaged = Iter.Value();
		const double ElapsedTime = CurrentTime - Damaged.LastFilteredHitTime;
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

void UMassDamageSubsystem::RegisterForHealthChange(const FMassEntityHandle Entity, UGSCCoreComponent& CoreComponent)
{
	EntityToComponentMap.Add(Entity,CoreComponent);
	ComponentToEntityMap.Add(CoreComponent,Entity);
	//CoreComponent.OnTakeDamage.AddDynamic(this,&UMassDamageSubsystem::OnDamageCallBack);
	//UE_LOG(LogTemp,Warning,TEXT("S1 %d "),ComponentToEntityMap.Num());
}

void UMassDamageSubsystem::UnregisterForHealthChange(FMassEntityHandle Entity, UGSCCoreComponent& CoreComponent)
{
	EntityToComponentMap.Remove(Entity);
	ComponentToEntityMap.Remove(&CoreComponent);
	CoreComponent.OnTakeDamage.RemoveAll(this);
}

void UMassDamageSubsystem::OnDamageCallBack(AActor* Causer,UGSCCoreComponent* OtherCoreComp,UGSCCoreComponent* PlayerCoreComp)
{
	//UE_LOG(LogTemp,Error,TEXT("Callback Damage"));
	const UWorld* World = GetWorld();
	check(World);
	UGSCCoreComponent* OtherCoreComponent = OtherCoreComp;
	const FMassEntityHandle Entity = ComponentToEntityMap.FindChecked(OtherCoreComp);
	FMassEntityHandle* PlayerEntity = ComponentToEntityMap.Find(PlayerCoreComp);
	const double CurrentTime = World->GetTimeSeconds();

	 //double HitResultMergeDuration = 1.;
	FMassDamageType* ExitsResult= DamageResult.Find(Entity);
	bool bProcessHit = true;
	if (ExitsResult)
	{
			ExitsResult->LastFilteredHitTime = CurrentTime;
			bProcessHit = false;
	}
	
	if (bProcessHit)
	{
		DamageResult.Add(Entity,{*PlayerEntity,CurrentTime});
	}
	SignalSubsystem->SignalEntity(UE::Mass::Signals::DamageTask,Entity);
	//UE_LOG(LogTemp,Warning,TEXT("S1 %d s2 %d"),Entity.Index,PlayerEntity->Index);
	
	
	/*FVector a= OtherCoreComp->GetOwner()->GetActorLocation()-PlayerCoreComp->GetOwner()->GetActorLocation();
	
	UMotionWarpingComponent* MotionWarping = OtherCoreComp->GetOwner()->FindComponentByClass<UMotionWarpingComponent>();
	MotionWarping->AddOrUpdateWarpTargetFromLocationAndRotation("Rotate",FVector::ZeroVector,
	                                                            FRotator(0.f, 90.f, 0.f) + a.ToOrientationRotator());
	UE_LOG(LogTemp,Warning,TEXT("YawAu %f Yaw %f "),OtherCoreComp->GetOwner()->GetActorRotation().Yaw,a.ToOrientationRotator().Yaw);*/
}






