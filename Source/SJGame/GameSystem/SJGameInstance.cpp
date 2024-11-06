// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/SJGameInstance.h"

#include "SJGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void USJGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(SJGameplayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(SJGameplayTags::InitState_DataAvailable, false, SJGameplayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(SJGameplayTags::InitState_DataInitialized, false, SJGameplayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(SJGameplayTags::InitState_GameplayReady, false, SJGameplayTags::InitState_DataInitialized);
	}
}
