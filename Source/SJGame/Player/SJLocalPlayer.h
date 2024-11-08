// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonLocalPlayer.h"
#include "SJLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SJGAME_API USJLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	
	//~ULocalPlayer interface
	virtual bool SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld) override;
	//~End of ULocalPlayer interface
};
