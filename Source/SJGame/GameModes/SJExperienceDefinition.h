// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SJExperienceDefinition.generated.h"

class UGameFeatureAction;
class USJExperienceActionSet;
/**
 * 
 */
UCLASS()
class SJGAME_API USJExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// List of Game Feature Plugins this experience wants to have active
	// 需要开启的GF列表
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;

	
	
	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	//需要开启的Action列表
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
	
	// List of additional action sets to compose into this experience
	// 将通用的Action组成一个整体,方便统一使用(不用在上面重复配置)
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TArray<TObjectPtr<USJExperienceActionSet>> ActionSets;
};
