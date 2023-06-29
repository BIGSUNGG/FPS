#pragma once

// Test
#define TEST_ADS false
#define TEST_RELOAD true

// Coliision
#define ECC_INTERACTION ECollisionChannel::ECC_GameTraceChannel1
#define ECC_ONLY_OBJECT ECollisionChannel::ECC_GameTraceChannel2
#define ECC_BULLET ECollisionChannel::ECC_GameTraceChannel3
#define ECC_SWING ECollisionChannel::ECC_GameTraceChannel4
#define ECC_ASSASSINATION ECollisionChannel::ECC_GameTraceChannel5

// Server
#define IS_SERVER() GetWorld()->IsNetMode(ENetMode::NM_DedicatedServer) || GetWorld()->IsNetMode(ENetMode::NM_ListenServer)

// Loga
#define KR_CALLINFO ((IS_SERVER() ? TEXT("Server :") : TEXT("Client :")) + FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define KR_LOG_S(Verbosity) UE_LOG(Kraver, Verbosity, TEXT("%s"), *KR_CALLINFO)
#define KR_LOG(Verbosity, Format, ...) UE_LOG(Kraver, Verbosity, TEXT("%s : %s : %s"), *KR_CALLINFO, (GetOwner() ? *this->GetOwner()->GetName() : *this->GetName()) ,*FString::Printf(Format, ##__VA_ARGS__))
#define KR_LOG_CHECK(Expr, ...) {if(!(Expr)) { KR_LOG(Error, TEXT("SSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__;}}
#define KR_LOG_VECTOR(Vector) { KR_LOG(Log, TEXT("X : %f , Y : %f , Z : %f"), Vector.X, Vector.Y, Vector.Z) }
#define KR_LOG_ROTATOR(Rotator) { KR_LOG(Log, TEXT("Pitch : %f , Yaw : %f , Roll : %f"), Rotator.Pitch, Rotator.Yaw, Rotator.Roll) }