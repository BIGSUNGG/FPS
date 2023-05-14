#pragma once

#include "EngineMinimal.h"
#include "Engine/DamageEvents.h"
#include "KraverStructHeaders.generated.h"

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
struct FKraverDamageEvent : public FDamageEvent
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FHitResult HitInfo;

	UPROPERTY()
	EKraverDamageType DamageType;

	UPROPERTY()
	float DamageImpulse = 0.f;

	UPROPERTY()
	bool bCanParried = false;

public:
	FVector GetHitDirection() const
	{ 
		FVector Result = (HitInfo.TraceEnd - HitInfo.TraceStart); 
		Result.Normalize();
		return Result; 
	}

	static const int32 ClassID = 3;

	virtual int32 GetTypeID() const override { return FKraverDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FKraverDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };
};
