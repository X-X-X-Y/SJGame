// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SJLocalPlayer.h"

bool USJLocalPlayer::SpawnPlayActor(const FString& URL, FString& OutError, UWorld* InWorld)
{
	const bool bResult = Super::SpawnPlayActor(URL, OutError, InWorld);

	return bResult;
}
