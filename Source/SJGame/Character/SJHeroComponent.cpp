// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJHeroComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "SJPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameSystem/SJGameplayTags.h"
#include "Player/SJLocalPlayer.h"
#include "Player/SJPlayerController.h"
#include "Player/SJPlayerState.h"
#include "SJPawnData.h"
#include "GameSystem/SJLogChannels.h"
#include "Input/SJInputComponent.h"
#include "Misc/UObjectToken.h"
#include "UserSettings/EnhancedInputUserSettings.h"

const FName USJHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName USJHeroComponent::NAME_ActorFeatureName("Hero");

#pragma region Start->End

void USJHeroComponent::OnRegister()
{
	Super::OnRegister();

	
	if (!GetPawn<APawn>())
	{
		UE_LOG(LogSJ, Error, TEXT("[ULyraHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("LyraHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("LyraHeroComponent");
			
			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));
				
			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

void USJHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(USJPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(SJGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void USJHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

#pragma endregion

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
	if (Params.FeatureName == USJPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == SJGameplayTags::InitState_DataInitialized)
		{
			//如果PawnEXComp表示其他组件已经完成初始化,进入下一初始化阶段
			CheckDefaultInitialization();
		}
	}
}

void USJHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		SJGameplayTags::InitState_Spawned, SJGameplayTags::InitState_DataAvailable,
		SJGameplayTags::InitState_DataInitialized, SJGameplayTags::InitState_GameplayReady
	};

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
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

	const USJLocalPlayer* LP = Cast<USJLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();
	if (const USJPawnExtensionComponent* PawnExtComp = USJPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const USJPawnData* PawnData = PawnExtComp->GetPawnData<USJPawnData>())
		{
			if (const USJInputConfig* InputConfig = PawnData->InputConfig)
			{
				//这里绑定Hero蓝图上的IMC
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}
							
							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				//这里绑定两个输入[1-基本天赋 2-技能输入] 需要通过InputConfig配置需要绑定哪些
				USJInputComponent* SJGameIC = Cast<USJInputComponent>(PlayerInputComponent);
				if (ensureMsgf(SJGameIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to USJInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					SJGameIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					SJGameIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
					
					SJGameIC->BindNativeAction(InputConfig, SJGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					SJGameIC->BindNativeAction(InputConfig, SJGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					// LyraIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
					// LyraIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
					// LyraIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}
}

#pragma endregion

#pragma region Hero Input CallBack

void USJHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const USJPawnExtensionComponent* PawnExtComp = USJPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			//TODO:Comming
		}	
	}
}

void USJHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
}

#pragma region Hero NativeInput CallBack

void USJHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (ASJPlayerController* SJGameController = Cast<ASJPlayerController>(Controller))
	{
		// LyraController->SetIsAutoRunning(false);
	}
	
	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void USJHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}
	
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

#pragma endregion

#pragma endregion
