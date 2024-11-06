// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "SJGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SJGAME_API USJGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
protected:
	virtual void Init() override;
};
