// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "SJGameState.generated.h"

class USJExperienceManagerComponent;
/**
 * 
 */
UCLASS()
class SJGAME_API ASJGameState : public AModularGameStateBase
{
	GENERATED_BODY()

public:
	ASJGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


private:
	UPROPERTY()
	TObjectPtr<USJExperienceManagerComponent> ExperienceManagerComponent;
};
