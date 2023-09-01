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
	// Rpc
	UFUNCTION(Server, Reliable)
		virtual void Server_FireBulletResult(const TArray<FHitResult>& BulletHitResults);

	// Function
	virtual void FireBullet() override;
	virtual void FireBulletResult(const TArray<FHitResult>& BulletHitResults);
	TArray<FHitResult>  CalculateFireHit(FName ProfileName, FVector Spread = FVector(0, 0, 0));

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletDistance = 20000.f; // ÃÑ¾Ë ±æÀÌ
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletRadius = 0.f; // ÃÑ¾Ë ¹üÀ§
};
