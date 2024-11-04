// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

SJGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSJ, Log, All);
SJGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSJExperience, Log, All);
SJGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSJAbilitySystem, Log, All);
SJGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogSJTeams, Log, All);

SJGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);