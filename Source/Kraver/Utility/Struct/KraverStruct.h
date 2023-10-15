#pragma once

#include "EngineMinimal.h"
#include "Engine/DamageEvents.h"
#include "Kraver/Utility/Enum/KraverEnum.h"
#include "KraverStruct.generated.h"

enum class EKraverDamageType : uint8;

USTRUCT(BlueprintType)
struct FAssassinateInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UAnimMontage* AssassinatedMontagesTpp;
	UPROPERTY()
		UAnimMontage* AssassinatedMontagesFpp;
};

// 크로스헤어의 이미지를 가지는 구조체
USTRUCT(BlueprintType)
struct FCrosshairsPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	class UTexture2D* CrosshairsLeft;
	class UTexture2D* CrosshairsRight;
	class UTexture2D* CrosshairsTop;
	class UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
};

USTRUCT(BlueprintType)
struct FKraverDamageResult
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bAlreadyDead = false;
	UPROPERTY()
	bool bCreatureDead = false;
	UPROPERTY()
	bool bCritical = false;
	UPROPERTY()
	int ActualDamage = 0;
};

UCLASS(MinimalAPI, const, Blueprintable, BlueprintType)
class UKraverDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AttackType)
	EKraverDamageType AttackType = EKraverDamageType::UNKWOWN;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AttackType)
	bool bCanParried = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AttackType)
	bool bCanHeadShot = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AttackType)
	bool bCanSimulate = true;
};

USTRUCT(BlueprintType)
struct FTeamInfo
{
	GENERATED_BODY()

public:
	static bool CheckIsTeam(ETeam ATeam, ETeam BTeam)
	{
		if (ATeam == ETeam::TEAM_ALL || BTeam == ETeam::TEAM_ALL)
			return true;
		if (ATeam == ETeam::ENEMY_ALL || BTeam == ETeam::ENEMY_ALL)
			return false;

		return ATeam == BTeam;
	}
	static bool CheckIsEnemy(ETeam ATeam, ETeam BTeam) { return !CheckIsTeam(ATeam, BTeam); }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team", meta = (AllowPrivateAccess = "true"))
	ETeam CurTeam = ETeam::ENEMY_ALL;
};

USTRUCT(BlueprintType)
struct FGraphicSettingInfo
{
	GENERATED_BODY()

public:
	int32 TextureQuality = 0;
	int32 ShadowQuality = 0;
	int32 AntiAliasingQuality = 0;
	int32 PostProcessingQuality = 0;
	int32 ViewDistanceQuality = 0;
	bool bEnableVSync = false;

public:
	bool operator == (FGraphicSettingInfo InInfo)
	{
		if (InInfo.TextureQuality == TextureQuality &&
			InInfo.ShadowQuality == ShadowQuality &&
			InInfo.AntiAliasingQuality == AntiAliasingQuality &&
			InInfo.PostProcessingQuality == PostProcessingQuality &&
			InInfo.ViewDistanceQuality == ViewDistanceQuality &&
			InInfo.bEnableVSync == bEnableVSync
			)
			return true;

		return false;
	}

	bool operator != (FGraphicSettingInfo InInfo) { return !operator==(InInfo); }

}; 

USTRUCT(BlueprintType)
struct FDisplaySettingInfo
{
	GENERATED_BODY()

public:
	EWindowMode::Type WinMode = EWindowMode::Type::Fullscreen;
	float MaxFrame = 0.f;

public:
	bool operator == (FDisplaySettingInfo InInfo)
	{
		if (InInfo.WinMode == WinMode &&
			InInfo.MaxFrame == MaxFrame
			)
			return true;

		return false;
	}

};