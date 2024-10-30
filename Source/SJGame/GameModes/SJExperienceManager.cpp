// Fill out your copyright notice in the Description page of Project Settings.


#include "SJExperienceManager.h"

void USJExperienceManager::OnPlayInEditorBegun()
{
}

void USJExperienceManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		USJExperienceManager* ExperienceManagerSubsystem = GEngine->GetEngineSubsystem<USJExperienceManager>();
		check(ExperienceManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = ExperienceManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool USJExperienceManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	return true;
}
