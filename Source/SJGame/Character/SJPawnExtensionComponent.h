// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "SJPawnExtensionComponent.generated.h"

class USJPawnData;
/**
 * 
 */
UCLASS()
class SJGAME_API USJPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
	
	/** Returns the pawn extension component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "SJGame|Pawn")
	static USJPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<USJPawnExtensionComponent>() : nullptr); }
	
	/** Sets the current pawn data */
	void SetPawnData(const USJPawnData* InPawnData);

	void SetupPlayerInputComponent();
	
protected:
	/** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	//TODO:ReplicatedUsing = OnRep_PawnData
	UPROPERTY(EditInstanceOnly, Category = "SJGame|Pawn")
	TObjectPtr<const USJPawnData> PawnData;
};
