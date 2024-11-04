// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SJPawnData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "SJGame Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class SJGAME_API USJPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SJGame|Pawn")
	TSubclassOf<APawn> PawnClass;
};
