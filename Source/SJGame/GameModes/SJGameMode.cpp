// Fill out your copyright notice in the Description page of Project Settings.


#include "SJGameMode.h"

#include "SJExperienceDefinition.h"
#include "SJExperienceManagerComponent.h"
#include "SJWorldSettings.h"
#include "GameSystem/SJAssetManager.h"
#include "GameSystem/SJLogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SJPlayerState.h"
#include "Character/SJPawnData.h"
#include "Character/SJPawnExtensionComponent.h"

#pragma region AGameModeBase Overried

void ASJGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Wait for the next frame to give time to initialize startup settings
	// 下一帧开始加载Experience
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ASJGameMode::InitGameState()
{
	Super::InitGameState();

	// Listen for the experience load to complete	
	USJExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USJExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnSJExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* ASJGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const USJPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}
	
	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ASJGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (USJPawnExtensionComponent* PawnExtComp = USJPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const USJPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogSJ, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}

				SpawnedPawn->FinishSpawning(SpawnTransform);

				return SpawnedPawn;
			}
		}
		else
		{
			UE_LOG(LogSJ, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogSJ, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}
	
	return nullptr;
}

void ASJGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	//判断设置新玩家时Experience是否加载中
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

#pragma endregion


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

void ASJGameMode::OnExperienceLoaded(const USJExperienceDefinition* CurrentExperience)
{
	// Spawn any players that are already attached
	//@TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	// 这里再次重置玩家是因为Experience未加载完找不到Player,Experience加载完后再次重置
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

#pragma endregion

#pragma region SJGameMode Public Feature

const USJPawnData* ASJGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const ASJPlayerState* SJGamePS = InController->GetPlayerState<ASJPlayerState>())
		{
			if (const USJPawnData* PawnData = SJGamePS->GetPawnData<USJPawnData>())
			{
				return PawnData;
			}
		}
	}
	
	check(GameState);
	USJExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USJExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const USJExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience && Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}
		
		// Experience is loaded and there's still no pawn data, fall back to the default for now
		return USJAssetManager::Get().GetDefaultPawnData();
	}

	//数据未完成初始化,可能需要等待Experience完成加载
	return nullptr;
}

bool ASJGameMode::IsExperienceLoaded() const
{
	check(GameState);
	USJExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USJExperienceManagerComponent>();
	check(ExperienceComponent);

	return ExperienceComponent->IsExperienceLoaded();
}

#pragma endregion 