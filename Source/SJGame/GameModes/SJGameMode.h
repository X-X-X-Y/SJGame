// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "SJGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SJGAME_API ASJGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
	
	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	//~End of AGameModeBase interface

protected:
	void HandleMatchAssignmentIfNotExpectingOne();
};
