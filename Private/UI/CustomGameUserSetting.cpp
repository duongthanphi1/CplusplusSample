// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CustomGameUserSetting.h"

UCustomGameUserSetting* UCustomGameUserSetting::GetCustomGameUserSetting()
{
	return Cast<UCustomGameUserSetting>(UGameUserSettings::GetGameUserSettings());
}
