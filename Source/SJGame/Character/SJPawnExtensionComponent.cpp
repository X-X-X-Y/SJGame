// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJPawnExtensionComponent.h"

#include "GameSystem/SJGameplayTags.h"
#include "GameSystem/SJLogChannels.h"

#pragma region IGameFrameworkInitStateInterface interface

void USJPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = {
		SJGameplayTags::InitState_Spawned, SJGameplayTags::InitState_DataAvailable,
		SJGameplayTags::InitState_DataInitialized, SJGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

#pragma endregion



void USJPawnExtensionComponent::SetPawnData(const USJPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();
	
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (PawnData)
	{
		// UE_LOG(LogSJ, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."),
		//        *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;
	Pawn->ForceNetUpdate();
	CheckDefaultInitialization();
}

void USJPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}
