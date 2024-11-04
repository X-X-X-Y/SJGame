// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "SJGameMode.generated.h"

class USJPawnData;
class USJExperienceDefinition;
/**
 * 
 */
UCLASS()
class SJGAME_API ASJGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SJGame|Pawn")
	const USJPawnData* GetPawnDataForController(const AController* InController) const;
	
	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void InitGameState() override;
	//~End of AGameModeBase interface

protected:
	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
	void OnExperienceLoaded(const USJExperienceDefinition* CurrentExperience);
};
