// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SJInputConfig.h"

#include "GameSystem/SJLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SJInputConfig)

const UInputAction* USJInputConfig::FindNativeInputActionForTag(FGameplayTag InputTag, bool bLogIfNotFound) const
{
	for (const FSJInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogIfNotFound)
	{
		UE_LOG(LogSJ, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* USJInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
	bool bLogIfNotFound) const
{
	for (const FSJInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogIfNotFound)
	{
		UE_LOG(LogSJ, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
