// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "SJCharacter.generated.h"

class USJPawnExtensionComponent;
/**
 * 
 */
UCLASS()
class SJGAME_API ASJCharacter : public AModularCharacter
{
	GENERATED_BODY()

public:

	ASJCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SJGame|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USJPawnExtensionComponent> PawnExtComponent;
};
