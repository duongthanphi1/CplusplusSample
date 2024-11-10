// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/SimpleRandomMovementTrait.h"

#include "MassEntityTemplateRegistry.h"
#include "Mass/SimpleRandomMovementFragment.h"

void USimpleRandomMovementTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FSimpleRandomMovementFragment>();
}
