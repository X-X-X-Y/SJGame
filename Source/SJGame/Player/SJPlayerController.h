// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonPlayerController.h"
#include "SJPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SJGAME_API ASJPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
		
	//~APlayerController interface
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface
};
