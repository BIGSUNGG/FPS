// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Actor/Weapon/Gun/Gun.h"
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
	virtual void FireBullet() override;
	TArray<FHitResult>  CalculateFireHit(FName ProfileName, FVector Spread = FVector(0, 0, 0));

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletDistance = 10000.f; // �Ѿ� ����
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		float BulletRadius = 0.f; // �Ѿ� ����
};
