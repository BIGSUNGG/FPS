// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include Gun_h
#include "HitscanGun.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AHitscanGun : public AGun
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	// Function
	virtual void OnServer_FireBullet() override;
	virtual void OnServer_FireBulletResult(const TArray<FHitResult>& BulletHitResults);

	TArray<FHitResult> CalculateFireHit(FName ProfileName, FVector Spread = FVector(0, 0, 0)); // 발사 결과

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	float BulletDistance = 20000.f; // 총알 길이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	float BulletRadius = 0.f; // 총알 범위
};
