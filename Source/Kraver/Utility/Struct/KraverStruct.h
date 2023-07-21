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
	int ActualDamage = 0.f;
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