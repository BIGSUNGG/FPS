// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Actor/Weapon/Gun/HitscanGun/HitscanGun.h"
#include "ProjectileGun.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AProjectileGun : public AHitscanGun
{
	GENERATED_BODY()
	
protected:
	virtual void FireBullet() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Bullet", meta = (AllowPrivateAccess = "true"))
		UClass* BulletClass = nullptr; // √—æÀ ±Ê¿Ã

};
