// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "CustomGameUserSetting.generated.h"

/**
 * 
 */
UCLASS()
class RPGGAME_API UCustomGameUserSetting : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UCustomGameUserSetting* GetCustomGameUserSetting();

	UPROPERTY(Config, BlueprintReadWrite)
	bool bAutoTarget;
protected:
	
	
};
