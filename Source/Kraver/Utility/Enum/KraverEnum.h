// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "KraverEnum.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	IDLE		UMETA(DisplayName = "IDLE"),
	EQUIPPED_UNHOLSTER	UMETA(DisplayName = "EQUIPPED_UNHOLSTER"),
	EQUIPPED_HOLSTER	UMETA(DisplayName = "EQUIPPED_HOLSTER"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE	UMETA(DisplayName = "NONE"),
	GUN		UMETA(DisplayName = "GUN"),
	MELEE   UMETA(DisplayName = "MELEE"),
};

// 캐릭터의 AdvancedMovement 상태를 가지는 enum class
UENUM(BlueprintType)
enum class EWallRunState : uint8
{
	NONE				UMETA(DisplayName = "NONE"),
	WALLRUN_RIGHT		UMETA(DisplayName = "WALLRUN_RIGHT"),
	WALLRUN_LEFT		UMETA(DisplayName = "WALLRUN_LEFT"),
	WALLRUN_VERTICAL	UMETA(DisplayName = "WALLRUN_VERTICAL"),
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

UENUM(BlueprintType)
enum class EKraverDamageType : uint8
{
	UNKWOWN			UMETA(DisplayName = "UNKWOWN"),
	SWING			UMETA(DisplayName = "SWING"),
	BULLET			UMETA(DisplayName = "BULLET"),
	EXPLOSION		UMETA(DisplayName = "EXPLOSION"),
	ASSASSINATION	UMETA(DisplayName = "ASSASSINATION"),
};

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left		UMETA(DisplayName = "Turning Left"),
	ETIP_Right		UMETA(DisplayName = "Turning Right"),
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),
	ETIP_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ETeam : uint8
{
	NONE		UMETA(DisplayName = "NONE"),
	TEAM_ALL	UMETA(DisplayName = "TEAM_ALL"),
	ENEMY_ALL	UMETA(DisplayName = "ENEMY_ALL"),
	RED			UMETA(DisplayName = "RED"),
	BLUE		UMETA(DisplayName = "BLUE"),
};

UENUM(BlueprintType)
enum class ESettingMenuState : uint8
{
	DISPLAY		UMETA(DisplayName = "DISPLAY"),
	GRAPHIC		UMETA(DisplayName = "GRAPHIC"),
};

UENUM(BlueprintType)
enum class EValueState : uint8
{
	NORMAL			UMETA(DisplayName = "NORMAL"),
	INCREASING		UMETA(DisplayName = "INCREASING"),
	DECREASING		UMETA(DisplayName = "DECREASING"),
};
