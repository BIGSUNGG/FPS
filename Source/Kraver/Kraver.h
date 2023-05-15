// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Kraver/Utility/UnrealHeaders.h"
#include "Kraver/Utility/KraverDelegate.h"
#include "Kraver/Utility/KraverEnum.h"
#include "Kraver/Utility/KraverStructHeaders.h"

using namespace std;

DECLARE_LOG_CATEGORY_EXTERN(Kraver, Log, All);

// Coliision
#define ECC_INTERACTION ECollisionChannel::ECC_GameTraceChannel1
#define ECC_ONLY_OBJECT ECollisionChannel::ECC_GameTraceChannel2
#define ECC_BULLET ECollisionChannel::ECC_GameTraceChannel3
#define ECC_SWING ECollisionChannel::ECC_GameTraceChannel4
#define ECC_ASSASSINATION ECollisionChannel::ECC_GameTraceChannel5

// Server
#define IS_SERVER() GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer) || GetWorld()->IsNetMode(ENetMode::NM_ListenServer)

// Log
#define KR_CALLINFO ((IS_SERVER() ? TEXT("Server :") : TEXT("Client :")) + FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define KR_LOG_S(Verbosity) UE_LOG(Kraver, Verbosity, TEXT("%s"), *KR_CALLINFO)
#define KR_LOG(Verbosity, Format, ...) UE_LOG(Kraver, Verbosity, TEXT("%s : %s : %s"), *KR_CALLINFO, *this->GetName() ,*FString::Printf(Format, ##__VA_ARGS__))
#define KR_LOG_CHECK(Expr, ...) {if(!(Expr)) { KR_LOG(Error, TEXT("SSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}
#define KR_LOG_VECTOR(Vector) { KR_LOG(Log, TEXT("X : %f , Y : %f , Z : %f"), Vector.X, Vector.Y, Vector.Z) }

// Function
bool LineTraceMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam); 
bool SweepMultiByChannel_ExceptWorldObject(UWorld* World, TArray<struct FHitResult>& OutHits, const FVector& Start, const FVector& End, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionShape& CollisionShape, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);