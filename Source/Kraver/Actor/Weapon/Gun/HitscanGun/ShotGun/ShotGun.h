// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include HitscanGun_h
#include "ShotGun.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AShotGun : public AHitscanGun
{
	GENERATED_BODY()

protected:
	virtual void OnServer_FireBullet() override;

public:
	// Getter Setter
	virtual float CalculateCurSpread();
	virtual bool ShouldApplySpread() { return true; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Combat|Ammo", meta = (AllowPrivateAccess = "true"))
	int64 NumberOfBullet = 1; // 한번에 발사할 총알의 개수 
};
