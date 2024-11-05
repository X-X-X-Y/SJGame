// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJCharacter.h"

#include "SJCharacterMovementComponent.h"
#include "SJPawnExtensionComponent.h"
#include "Camera/SJCameraComponent.h"

ASJCharacter::ASJCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<USJCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PawnExtComponent = CreateDefaultSubobject<USJPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	CameraComponent = CreateDefaultSubobject<USJCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
}



#pragma region ASJCharacter Override

void ASJCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// PawnExtComponent->SetupPlayerInputComponent();
}

#pragma endregion