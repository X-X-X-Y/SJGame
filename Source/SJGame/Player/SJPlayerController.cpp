// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SJPlayerController.h"

#include "SJLocalPlayer.h"
#include "GameSystem/SJLogChannels.h"

#pragma region APlayerController Interface

void ASJPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const USJLocalPlayer* BaiLuLocalPlayer = Cast<USJLocalPlayer>(InPlayer))
	{
		UE_LOG(LogSJ, Log, TEXT("Set local player"))
	}
}

void ASJPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

#pragma endregion