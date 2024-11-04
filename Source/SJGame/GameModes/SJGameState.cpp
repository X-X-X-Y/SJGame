// Fill out your copyright notice in the Description page of Project Settings.


#include "SJGameState.h"

#include "SJExperienceManagerComponent.h"

ASJGameState::ASJGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ExperienceManagerComponent = CreateDefaultSubobject<USJExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
