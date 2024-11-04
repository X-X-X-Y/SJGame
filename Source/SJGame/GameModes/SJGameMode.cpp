// Fill out your copyright notice in the Description page of Project Settings.


#include "SJGameMode.h"

#include "SJExperienceDefinition.h"
#include "SJExperienceManagerComponent.h"
#include "SJWorldSettings.h"
#include "GameSystem/SJAssetManager.h"
#include "GameSystem/SJLogChannels.h"
#include "Kismet/GameplayStatics.h"

void ASJGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Wait for the next frame to give time to initialize startup settings
	// 下一帧开始加载Experience
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

#pragma region Experience Methods

void ASJGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins) 多种加载Experience的方式
	//  - Matchmaking assignment (if present)
	//  - URL Options override
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Dedicated server
	//  - Default experience

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(USJExperienceDefinition::StaticClass()->GetFName()),
		                               FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	//TODO:DeveloperSettings Set Experience

	// see if the command line wants to set the experience
	if (!ExperienceId.IsValid())
	{
		FString ExperienceFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
		{
			ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
			if (!ExperienceId.PrimaryAssetType.IsValid())
			{
				ExperienceId = FPrimaryAssetId(FPrimaryAssetType(USJExperienceDefinition::StaticClass()->GetFName()),
				                               FName(*ExperienceFromCommandLine));
			}
			ExperienceIdSource = TEXT("CommandLine");
		}
	}

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (ASJWorldSettings* TypedWorldSettings = Cast<ASJWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	USJAssetManager& AssetManager = USJAssetManager::Get();
	if (FAssetData Dummy; ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	{
		UE_LOG(LogSJExperience, Error,
		       TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"),
		       *ExperienceId.ToString());
		ExperienceId = FPrimaryAssetId();
	}
	
	// Final fallback to the default experience
	if (!ExperienceId.IsValid())
	{
		//TODO: Pull this from a config setting or something
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType("SJExperienceDefinition"), FName("B_SJDefaultExperience"));
		ExperienceIdSource = TEXT("Default");
	}
	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void ASJGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogSJExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		USJExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USJExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogSJExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}

#pragma endregion

