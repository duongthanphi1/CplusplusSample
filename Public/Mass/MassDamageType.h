#pragma once
#include "MassEntityTypes.h"

#include "MassDamageType.generated.h"

USTRUCT()
struct FMassDamageType
{
	GENERATED_BODY()
	
	FMassDamageType() = default;

	FMassDamageType(const FMassEntityHandle OtherEntity, const double Time)
	: OtherEntity(OtherEntity)
	, HitTime(Time)
	, LastFilteredHitTime(Time)
{
}

bool IsValid() const { return OtherEntity.IsValid(); }
	
FMassEntityHandle OtherEntity;

/** Time when first hit was received. */
double HitTime = 0.;

/** Time used for filtering frequent hits. */
double LastFilteredHitTime = 0.;
};

USTRUCT()
struct FMassInteractionType
{
	GENERATED_BODY()
	
	FMassInteractionType() = default;

	FMassInteractionType(const FMassEntityHandle OtherEntity, const double Time)
	: OtherEntity(OtherEntity)
	, InteractTime(Time)
	, LastFilteredInteractTime(Time)
	{
	}

	bool IsValid() const { return OtherEntity.IsValid(); }
	
	FMassEntityHandle OtherEntity;

	/** Time when first hit was received. */
	double InteractTime = 0.;

	/** Time used for filtering frequent hits. */
	double LastFilteredInteractTime = 0.;
};