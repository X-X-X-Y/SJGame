// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturePluginOperationResult.h"
#include "Components/GameStateComponent.h"
#include "SJExperienceManagerComponent.generated.h"

class USJExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded, const USJExperienceDefinition* /*Experience*/);

enum class ESJExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class SJGAME_API USJExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);
	

private:
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	
private:
	TObjectPtr<const USJExperienceDefinition> CurrentExperience;
	ESJExperienceLoadState LoadState = ESJExperienceLoadState::Unloaded;
	TArray<FString> GameFeaturePluginURLs;
	int32 NumGameFeaturePluginsLoading = 0;

	/**
	* Delegate called when the experience has finished loading just before others
	* (e.g., subsystems that set up for regular gameplay)
	*/
	FOnLyraExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnLyraExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnLyraExperienceLoaded OnExperienceLoaded_LowPriority;
};
