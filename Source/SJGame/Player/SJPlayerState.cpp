// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SJPlayerState.h"

#include "GameModes/SJExperienceManagerComponent.h"
#include "GameModes/SJGameMode.h"
#include "GameSystem/SJLogChannels.h"
#include "Net/Core/PushModel/PushModel.h"

void ASJPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ASJPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
	// check(AbilitySystemComponent);
	// AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		USJExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<USJExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnSJExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

void ASJPlayerState::OnExperienceLoaded(const USJExperienceDefinition* USJExperienceDefinition)
{
	if (ASJGameMode* LyraGameMode = GetWorld()->GetAuthGameMode<ASJGameMode>())
	{
		if (const USJPawnData* NewPawnData = LyraGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogSJ, Error, TEXT("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}
	}
}

void ASJPlayerState::SetPawnData(const USJPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		// UE_LOG(LogSJ, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	// MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	// for (const ULyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	// {
	// 	if (AbilitySet)
	// 	{
	// 		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
	// 	}
	// }
	//
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);
	//
	ForceNetUpdate();
}