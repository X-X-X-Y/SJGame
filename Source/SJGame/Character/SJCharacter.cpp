// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SJCharacter.h"

#include "SJCharacterMovementComponent.h"
#include "SJPawnExtensionComponent.h"

ASJCharacter::ASJCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<USJCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PawnExtComponent = CreateDefaultSubobject<USJPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}
