// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "SJAssetManager.generated.h"


struct FSJBundles
{
	static const FName Equipped;
};

/**
 * 
 */
UCLASS()
class SJGAME_API USJAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Returns the AssetManager singleton object.
	static USJAssetManager& Get();
	
};
