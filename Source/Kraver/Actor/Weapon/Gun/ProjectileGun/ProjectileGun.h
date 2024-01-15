// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include HitscanGun_h
#include "ProjectileGun.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AProjectileGun : public AGun
{
	GENERATED_BODY()
	
protected:
	virtual void OnServer_FireBullet() override;

	virtual void OnServer_SpawnBullet(FVector Location, FRotator Rotation);

	// Delegate
	UFUNCTION()
	virtual void OnBulletImpactEvent(AActor* Bullet, AActor* OtherActor, UPrimitiveComponent* OtherComponent, const FHitResult& Hit);

	// Rpc
	virtual void Multicast_ImpactBullet_Implementation(FVector ImpactPos) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Bullet", meta = (AllowPrivateAccess = "true"))
	UClass* BulletClass = nullptr; // ÃÑ¾Ë Å¬·¡½º

};
