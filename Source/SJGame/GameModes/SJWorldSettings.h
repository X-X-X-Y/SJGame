// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "SJWorldSettings.generated.h"

class USJExperienceDefinition;
/**
 * 
 */
UCLASS()
class SJGAME_API ASJWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:
	//Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	//没有User-Experience则使用WorldSetting中默认的Experience
	FPrimaryAssetId GetDefaultGameplayExperience() const;
	
protected:
	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<USJExperienceDefinition> DefaultGameplayExperience;
};	
