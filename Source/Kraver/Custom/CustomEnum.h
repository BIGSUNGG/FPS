// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomEnum.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	NONE   UMETA(DisplayName = "NONE"),
	EQUIPPED   UMETA(DisplayName = "EQUIPPED"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE   UMETA(DisplayName = "NONE"),
	GUN   UMETA(DisplayName = "GUN"),
	MELEE   UMETA(DisplayName = "MELEE"),
};

// 캐릭터의 AdvancedMovement 상태를 가지는 enum class
UENUM(BlueprintType)
enum class EWallRunState : uint8
{
	NONE		UMETA(DisplayName = "NONE"),
	WALLRUN_RIGHT UMETA(DisplayName = "WALLRUN_RIGHT"),
	WALLRUN_LEFT UMETA(DisplayName = "WALLRUN_LEFT"),
	WALLRUN_VERTICAL UMETA(DisplayName = "WALLRUN_VERTICAL"),
};

// 카메라 시점 종류를 가지는 enum class
UENUM(BlueprintType)
enum class EViewType : uint8
{
	FIRST_PERSON   UMETA(DisplayName = "FIRST_PERSON"),
	THIRD_PERSON   UMETA(DisplayName = "THIRD_PERSON"),
};

// 캐릭터의 움직임 상태를 가지는 enum class
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	WALK		UMETA(DisplayName = "WALK"),
	RUN			UMETA(DisplayName = "RUN"),
	SPRINT		UMETA(DisplayName = "SPRINT"),
};

UCLASS()
class KRAVER_API ACustomEnum : public AActor
{
	GENERATED_BODY()

};
