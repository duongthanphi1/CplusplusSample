// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "MassProcessor.h"
#include "SimpleRandomMovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API USimpleRandomMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()
public:
	USimpleRandomMovementProcessor();
protected:
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;
	
	virtual void ConfigureQueries() override;

private:
	FMassEntityQuery EntityQuery;
};

