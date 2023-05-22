// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Weapon/Gun/Gun.h"
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AShotGun : public AGun
{
	GENERATED_BODY()

protected:
	virtual void FireBullet() override;

public:
	// Getter Setter
	virtual bool GetbApplySpread() override { return true; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
		int64 NumberOfBullet = 1;
};
