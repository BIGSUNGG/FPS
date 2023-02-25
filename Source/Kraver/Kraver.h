// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Kraver, Log, All);

#define KR_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define KR_LOG_S(Verbosity) UE_LOG(Kraver, Verbosity, TEXT("%s"), *KR_CALLINFO)
#define KR_LOG(Verbosity, Format, ...) UE_LOG(Kraver, Verbosity, TEXT("%s : %s"), *KR_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))
#define KR_LOG_CHECK(Expr, ...) {if(!(Expr)) { KR_LOG(Error, TEXT("SSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}