// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/SJInputComponent.h"

USJInputComponent::USJInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void USJInputComponent::AddInputMappings(const USJInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void USJInputComponent::RemoveInputMappings(const USJInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void USJInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
