// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJPawnExtensionComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "GameSystem/SJGameplayTags.h"

const FName USJPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

#pragma region Moon Behavior

void USJPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("SJPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(USJPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one SJPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	
	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void USJPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(SJGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void USJPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// UninitializeAbilitySystem();
	UnregisterInitStateFeature();

	
	Super::EndPlay(EndPlayReason);
}

#pragma endregion

#pragma region IGameFrameworkInitStateInterface interface

bool USJPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == SJGameplayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == SJGameplayTags::InitState_Spawned && DesiredState == SJGameplayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == SJGameplayTags::InitState_DataAvailable && DesiredState == SJGameplayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, SJGameplayTags::InitState_DataAvailable);
	}
	else if (CurrentState == SJGameplayTags::InitState_DataInitialized && DesiredState == SJGameplayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void USJPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == SJGameplayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void USJPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == SJGameplayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

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

#pragma region PawnExtensionComponent Public Field

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

#pragma endregion

