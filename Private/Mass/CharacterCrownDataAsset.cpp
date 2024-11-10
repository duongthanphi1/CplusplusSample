// Fill out your copyright notice in the Description page of Project Settings.


#include "Mass/CharacterCrownDataAsset.h"

UAnimToTextureDataAsset* UCharacterCrownDataAsset::GetAnimtoTextureDataAsset(
	TArray<FCharacterDefinition> CharacterDefinitions)
{
	if (CharacterDefinition.IsEmpty())
	{
		return nullptr;
	}
	return CharacterDefinition.GetData()->AnimToTextureDataAsset;
}
