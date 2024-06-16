#pragma once

#include "HeaderMacro.h"

// Test
#define TEST_ADS false
#define TEST_RELOAD false

// Coliision

// Channel
#define ECC_INTERACTION ECollisionChannel::ECC_GameTraceChannel1
#define ECC_ONLY_OBJECT ECollisionChannel::ECC_GameTraceChannel2
#define ECC_CharacterMesh ECollisionChannel::ECC_GameTraceChannel3
#define ECC_ASSASSINATION ECollisionChannel::ECC_GameTraceChannel5

#define PROFILE_Swing "Swing"
#define PROFILE_Bullet "Bullet"
#define PROFILE_ControlArea "ControlArea"

#define IS_SERVER() UKismetSystemLibrary::IsServer(this)
#define IS_CLIENT() !IS_SERVER()

// Log
#define KR_CALLINFO ((IS_SERVER() ? TEXT("Server :") : TEXT("Client :")) + FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define KR_LOG_S(Verbosity) UE_LOG(Kraver, Verbosity, TEXT("%s"), *KR_CALLINFO)
#define KR_LOG(Verbosity, Format, ...) UE_LOG(Kraver, Verbosity, TEXT("%s : %s : %s"), *KR_CALLINFO, *this->GetName() ,*FString::Printf(Format, ##__VA_ARGS__))
#define KR_LOG_CHECK(Expr, ...) {if(!(Expr)) { KR_LOG(Error, TEXT("SSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}
#define KR_LOG_VECTOR(Vector) { KR_LOG(Log, TEXT("X : %f , Y : %f , Z : %f"), Vector.X, Vector.Y, Vector.Z) }
#define KR_LOG_ROTATOR(Rotator) { KR_LOG(Log, TEXT("Pitch : %f , Yaw : %f , Roll : %f"), Rotator.Pitch, Rotator.Yaw, Rotator.Roll) }

#define ERROR_IF_CALLED_ON_CLIENT() {if(!IS_SERVER()) { KR_LOG(Error, TEXT("Function called on client")); return; }}
#define ERROR_IF_CALLED_ON_CLIENT_PARAM(Value) {if(!IS_SERVER()) { KR_LOG(Error, TEXT("Function called on client")); return Value; }}

#define ERROR_IF_NOT_CALLED_ON_LOCAL() {if(!FindOwnerByClass<ACreature>(GetOwner())->IsLocallyControlled()) { KR_LOG(Error, TEXT("Function not called on local")); return; }}
#define ERROR_IF_NOT_CALLED_ON_LOCAL_PARAM(Value) {if(!FindOwnerByClass<ACreature>(GetOwner())->IsLocallyControlled()) { KR_LOG(Error, TEXT("Function not called on local")); return Value; }}
