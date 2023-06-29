// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Utility/Header/UnrealHeader.h"
#include "Kraver/Utility/Delegate/KraverDelegate.h"
#include "Kraver/Utility/Enum/KraverEnum.h"
#include "Kraver/Utility/Struct/KraverStruct.h"
#include "Kraver/Utility/Function/KraverFunction.h"

using namespace std;

DECLARE_LOG_CATEGORY_EXTERN(Kraver, Log, All);

// Typedef
using FWeaponPrimitiveInfo = TMap<FString, UPrimitiveComponent*>;
