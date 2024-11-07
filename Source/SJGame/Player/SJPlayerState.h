// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "GameModes/SJExperienceDefinition.h"
#include "SJPlayerState.generated.h"

class USJPawnData;
/**
 * 
 */
UCLASS()
class SJGAME_API ASJPlayerState : public AModularPlayerState
{
	GENERATED_BODY()

public:
	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const USJPawnData* USJPawnData);

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

private:

	void OnExperienceLoaded(const USJExperienceDefinition* USJExperienceDefinition);
	
protected:
	
	//TODO:UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const USJPawnData> PawnData;
};
