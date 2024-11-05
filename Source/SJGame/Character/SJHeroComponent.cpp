// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJHeroComponent.h"

#include "EnhancedInputSubsystems.h"
#include "SJPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameSystem/SJGameplayTags.h"
#include "Player/SJPlayerController.h"
#include "Player/SJPlayerState.h"

#pragma region IGameFrameworkInitStateInterface interface

bool USJHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == SJGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == SJGameplayTags::InitState_Spawned && DesiredState == SJGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ASJPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			ASJPlayerController* SJGamePC = GetController<ASJPlayerController>();
			
			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !SJGamePC || !SJGamePC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == SJGameplayTags::InitState_DataAvailable && DesiredState == SJGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ASJPlayerState* SJGamePS = GetPlayerState<ASJPlayerState>();

		return SJGamePS && Manager->HasFeatureReachedInitState(Pawn, USJPawnExtensionComponent::NAME_ActorFeatureName, SJGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == SJGameplayTags::InitState_DataInitialized && DesiredState == SJGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}
	return false;
}

void USJHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState)
{
	if (CurrentState == SJGameplayTags::InitState_DataAvailable && DesiredState ==
		SJGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ASJPlayerState* LyraPS = GetPlayerState<ASJPlayerState>();
		if (!ensure(Pawn && LyraPS))
		{
			return;
		}

		const USJPawnData* PawnData = nullptr;
		if (USJPawnExtensionComponent* PawnExtComp = USJPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			//TODO_Comming:Init GAS
		}

		if (ASJPlayerController* SJGamePC = GetController<ASJPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void USJHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	IGameFrameworkInitStateInterface::OnActorInitStateChanged(Params);
}

void USJHeroComponent::CheckDefaultInitialization()
{
	IGameFrameworkInitStateInterface::CheckDefaultInitialization();
}

#pragma endregion

#pragma region Hero Input

void USJHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	// const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PC->GetLocalPlayer());
	// check(LP);

	// UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	// check(Subsystem);
}

#pragma endregion

