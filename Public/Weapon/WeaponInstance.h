// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/UObjectGlobals.h"
#include "Equipment/EquipmentInstance.h"

#include "WeaponInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UWeaponInstance : public UEquipmentInstance
{
	GENERATED_BODY()

public:
	UWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UEquipmentInstance interface
	virtual void OnEquipped();
	virtual void OnUnequipped();
	//~End of UEquipmentInstance interface

protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AnimLayers")
	TSubclassOf<UAnimInstance> EquipLayer;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AnimLayers")
	TSubclassOf<UAnimInstance> UnEquipLayer;



	
};
