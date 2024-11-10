#pragma once

#include "MassEntityTypes.h"
#include "MassStateTreeTypes.h"
#include "MassDamageEvaluator.generated.h"

class UMassDamageSubsystem;

USTRUCT()
struct RPGGAME_API FMMassDamageEvaluatorInstancedData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Output)
	bool bGotDamage = false;

	UPROPERTY(VisibleAnywhere, Category = Output)
	FMassEntityHandle LastCauserEntity;
};

USTRUCT(meta = (DisplayName = "Mass Damage Eval"))
struct RPGGAME_API FMMassDamageEvaluator : public FMassStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FMMassDamageEvaluatorInstancedData;
	
protected:
	virtual bool Link(FStateTreeLinker& Linker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

	TStateTreeExternalDataHandle<UMassDamageSubsystem> DamageSubsystemHandle;
};