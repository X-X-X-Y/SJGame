// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturePluginOperationResult.h"
#include "Components/GameStateComponent.h"
#include "SJExperienceManagerComponent.generated.h"

class USJExperienceDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSJExperienceLoaded, const USJExperienceDefinition* /*Experience*/);

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

	// Ensures the delegate is called once the experience has been loaded,
	// However, if the experience has already loaded, calls the delegate immediately.
	void CallOrRegister_OnExperienceLoaded_HighPriority(FOnSJExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded(FOnSJExperienceLoaded::FDelegate&& Delegate);
	void CallOrRegister_OnExperienceLoaded_LowPriority(FOnSJExperienceLoaded::FDelegate&& Delegate);
	
	// Returns true if the experience is fully loaded
	bool IsExperienceLoaded() const;

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const USJExperienceDefinition* GetCurrentExperienceChecked() const;

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
	FOnSJExperienceLoaded OnExperienceLoaded_HighPriority;

	/** Delegate called when the experience has finished loading */
	FOnSJExperienceLoaded OnExperienceLoaded;

	/** Delegate called when the experience has finished loading */
	FOnSJExperienceLoaded OnExperienceLoaded_LowPriority;
};
