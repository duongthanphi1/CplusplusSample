#pragma once
#include "MassEntityTypes.h"

#include "SimpleRandomMovementFragment.generated.h"

USTRUCT()
struct FSimpleRandomMovementFragment : public FMassFragment
{
	GENERATED_BODY()
	
	//FSimpleRandomMovementFragment() =default;
	FVector Target;
};
